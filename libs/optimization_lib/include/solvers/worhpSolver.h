#pragma once

#include <libs/optimization_lib/include/utils.h>
#include <libs/optimization_lib/include/objective_functions/ObjectiveFunction.h>
#include <libs/optimization_lib/include/objective_functions/LagrangianLscmStArea.h>
#include <ext/worhp/worhp/worhp.h>

class worhpSolver
{
public:
	worhpSolver();
	~worhpSolver();
	int run(
		const MatrixXd& V,
		const MatrixX3i& F,
		const VectorXd& initialPoint
	);
private:
	LagrangianLscmStArea* function;
	int x;
	// Objective function
	void UserF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Function of constraints
	void UserG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Gradient of objective function
	void UserDF(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Jacobian of constraints
	void UserDG(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
	// Hessian of Lagrangian
	void UserHM(OptVar* opt, Workspace* wsp, Params* par, Control* cnt);
};