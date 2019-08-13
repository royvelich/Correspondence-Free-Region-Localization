#include <solvers/newton_solver.h>NewtonSolver::NewtonSolver(int maxIterations, double solveResidual, int maxLineSearchIterations)	: Solver(maxIterations, solveResidual, maxLineSearchIterations) {	}NewtonSolver::~NewtonSolver() {}void NewtonSolver::computeSearchDirection(ObjectiveFunction *function, const VectorXd &x, VectorXd& dx) {	VectorXd grad;	std::vector<Tripletd> Hess;	grad.resize(x.size());	grad.setZero();	Hess.clear();	function->addGradientTo(grad, x);	function->addHessianEntriesTo(Hess, x);	SparseMatrixd HessMat(x.size(), x.size());	HessMat.setFromTriplets(Hess.begin(), Hess.end());	Eigen::SimplicialLDLT<SparseMatrixd> solverLDLT;	solverLDLT.compute(HessMat);	dx = solverLDLT.solve(-1 * grad);}