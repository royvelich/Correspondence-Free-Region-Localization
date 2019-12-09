#include <solvers/NewtonSolver.h>

double NewtonSolver::step()
{
	objective->updateX(X);
	if(IsConstrObjFunc)
		f = objective->AugmentedValue(true);
	else
		f = objective->value(true);
	objective->gradient(g);
	objective->hessian();

#ifdef USE_PARDISO
	pardiso->update_a(objective->SS);
	try
	{
		pardiso->factorize();
	}
	catch (runtime_error& err)
	{
		cout << err.what();
		return -1;
	}
	VectorXd rhs = -g;
	pardiso->solve(rhs, p);
#else
	eigen_solver->factorize(objective->II, objective->JJ, objective->SS);
	VectorXd rhs = -g;
	p = eigen_solver->solve(rhs);
#endif
	return f;
}

bool NewtonSolver::test_progress()
{
	return true;
}

void NewtonSolver::internal_init()
{
#ifdef USE_PARDISO
	bool needs_init = pardiso == nullptr;

	if (needs_init)
	{
		pardiso = make_unique<PardisoSolver<vector<int>, vector<double>>>();
		pardiso->set_type(2, true);
	}
#else
	bool needs_init = eigen_solver == nullptr;

	if (needs_init)
	{
		eigen_solver = make_unique<EigenSparseSolver<vector<int>, vector<double>>>();
	}
#endif
	objective->updateX(X);
	g.resize(X.size());
	objective->hessian();

#ifdef USE_PARDISO
	if (needs_init)
	{
		pardiso->set_pattern(objective->II, objective->JJ, objective->SS);
		pardiso->analyze_pattern();
	}
#else
	if (needs_init)
	{
		eigen_solver->set_pattern(objective->II, objective->JJ, objective->SS);
		eigen_solver->analyze_pattern();
	}
#endif
}