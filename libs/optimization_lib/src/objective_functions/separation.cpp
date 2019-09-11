// STL includes
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
#include <limits>
#include <utility>

// LIBIGL includes
#include <igl/cat.h>

// Optimization lib includes
#include "objective_functions/separation.h"

// OpenMP
#include <omp.h>

Separation::Separation(const std::shared_ptr<MeshWrapper>& mesh_wrapper) :
	ObjectiveFunction(mesh_wrapper)
{
	// wedges energy
	igl::cat(1, EVvar1, EVvar2, Esep);
	Esept = Esep.transpose();

	for (int i = 0; i < Esept.outerSize(); ++i)
	{
		// no inner loop because there are only 2 nnz values per col
		Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
		int idx_xi = it.row();
		int idx_xj = (++it).row();
		std::pair<int, int> p(idx_xi, idx_xj);
		pair2ind.push_back(p);
		ind2pair.emplace(p, i);
		ind2pair.emplace(std::make_pair(p.second, p.first), i);
	}

	V2Vt = V2V.transpose();
	C2C = V2Vt * V2V;
	Eigen::SparseMatrix<double> I(V2V.cols(), V2V.cols());
	I.setIdentity();
	C2C -= I;
	C2C.prune(0, 0);

	connect_alphas = Eigen::VectorXd::Zero(Esep.rows());
	disconnect_alphas = Eigen::VectorXd::Ones(Esep.rows());

	edge_lenghts_per_pair = Eigen::VectorXd::Ones(Esept.cols());
	no_seam_constraints_per_pair = Eigen::VectorXd::Zero(Esept.cols());
}

Separation::~Separation()
{

}

void Separation::InitializeHessian(const std::shared_ptr<MeshWrapper>& mesh_wrapper, std::vector<int>& II, std::vector<int>& JJ, std::vector<double>& SS)
{
	auto n = GetMeshWrapper().GetImageVertices().rows();

	II.clear();
	JJ.clear();

	auto PushPair = [&](int i, int j) { 
		II.push_back(i);
		JJ.push_back(j); 
	};

	for (int i = 0; i < Esept.outerSize(); ++i)
	{
		Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
		int idx_xi = it.row();
		int idx_xj = (++it).row();

		// The indices in the small hessians are setup like this:
		// xi, xi+n, xj, xj+n from top to bottom and left to right
		// we traverse only the upper diagonal of each 4x4 hessian
		// and thus store 10 values, gathered in column order.

		// First column
		PushPair(idx_xi, idx_xi);

		// Second column
		PushPair(idx_xi, idx_xi + n);
		PushPair(idx_xi + n, idx_xi + n);

		// Third column
		PushPair(idx_xi, idx_xj);
		//PushPair(idx_xi + n,	idx_xj);
		PushPair(idx_xj, idx_xi + n);
		PushPair(idx_xj, idx_xj);

		// Fourth column
		PushPair(idx_xi, idx_xj + n);
		PushPair(idx_xi + n, idx_xj + n);
		PushPair(idx_xj, idx_xj + n);
		PushPair(idx_xj + n, idx_xj + n);
	}

	SS = std::vector<double>(II.size(), 0.);
}

void Separation::CalculateValue(const Eigen::MatrixX2d& X, double& f)
{
	auto n = GetMeshWrapper().GetImageVertices().rows();

	EsepP = Esep * X;
	EsepP_squared_rowwise_sum = EsepP.array().pow(2.0).rowwise().sum();

	for (int i = 0; i < Esept.outerSize(); ++i)
	{
		// no inner loop because there are only 2 nnz values per col
		Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
		int idx_xi = it.row();
		int idx_xj = (++it).row();
	}

	EsepP_squared_rowwise_sum_plus_delta = EsepP_squared_rowwise_sum.array() + delta;
	f_per_pair = EsepP_squared_rowwise_sum.cwiseQuotient(EsepP_squared_rowwise_sum_plus_delta);

	// store values before taking painting into account
	f_sep_per_pair = f_per_pair;

	// add attraction force from painting
	// alpha * ||xi - xj||^2
	f_per_pair += (connect_alphas + no_seam_constraints_per_pair).cwiseProduct(EsepP_squared_rowwise_sum);

	// apply distraction force from painting
	// f -> alpha * f
	f_per_pair = f_per_pair.cwiseProduct(disconnect_alphas);

	// add edge length factor
	f_per_pair = f_per_pair.cwiseProduct(edge_lenghts_per_pair);

	// if a pair shall not be a seam, it should have a high value
	// f_per_pair = f_per_pair.cwiseProduct(no_seam_constraints_per_pair);

	// sum everything up
	f = f_per_pair.sum();
}

void Separation::CalculateGradient(const Eigen::MatrixX2d& X, Eigen::VectorXd& g)
{
	Eigen::MatrixX2d ge;

	Eigen::VectorXd d_vec = Eigen::VectorXd::Constant(EsepP_squared_rowwise_sum.rows(), delta);
	Eigen::VectorXd x_plus_d = EsepP_squared_rowwise_sum + d_vec;
	Eigen::VectorXd d = d_vec.cwiseQuotient(x_plus_d.cwiseAbs2());
	Eigen::VectorXd dconn_e_disconn = (d + connect_alphas + no_seam_constraints_per_pair).cwiseProduct(edge_lenghts_per_pair).cwiseProduct(disconnect_alphas);
	ge = 2.0 * Esept * dconn_e_disconn.asDiagonal() * EsepP;

	g = Eigen::Map<Eigen::VectorXd>(ge.data(), 2.0 * ge.rows(), 1);
}

void Separation::CalculateHessian(const Eigen::MatrixX2d& X, std::vector<double>& SS)
{
	int n = X.rows();
	int threads = omp_get_max_threads();

	// no inner loop because there are only 2 nnz values per col
	#pragma omp parallel for num_threads(threads)
	for (int i = 0; i < Esept.outerSize(); ++i)
	{
		int tid = omp_get_thread_num();

		Eigen::Vector2d xi, xj;
		Eigen::Matrix4d sh;

		int idx_xi;
		int idx_xj;
		int factor;

		Eigen::SparseMatrix<double>::InnerIterator it(Esept, i);
		idx_xi = it.row();
		factor = it.value();
		idx_xj = (++it).row();
		xi = X.row(idx_xi);
		xj = X.row(idx_xj);
		FindSingleHessian(xi, xj, sh);
		sh *= factor;

		// add the additional factors like coloring and edge splitting/merging
		Eigen::Matrix4d Esep4;
		Esep4 << 1,  0, -1,  0,
				 0,  1,  0, -1,
				-1,  0,  1,  0,
				 0, -1,  0,  1;

		sh += Esep4 * (connect_alphas(i) + no_seam_constraints_per_pair(i));
		sh *= edge_lenghts_per_pair(i);
		sh *= disconnect_alphas(i);

		int ind = 10 * i;
		for (int a = 0; a < 4; ++a)
		{
			for (int b = 0; b <= a; ++b)
			{
				SS[ind++] = sh(b, a);
			}
		}
	}
}

void Separation::FindSingleHessian(const Eigen::Vector2d& xi, const Eigen::Vector2d& xj, Eigen::Matrix4d& h)
{
	bool speedup = true;
	Eigen::Vector2d dx = xi - xj;
	Eigen::Vector4d dxx;
	dxx << dx, -dx;
	double t = 0.5*dx.squaredNorm();
	double fp, fpp;

	fp = delta / ((t + delta) * (t + delta));
	Eigen::Matrix4d Esep4;
	Esep4 << 1,  0, -1,  0,
			 0,  1,  0, -1,
			-1,  0,  1,  0,
			 0, -1,  0,  1;

	h = fp * Esep4;
}