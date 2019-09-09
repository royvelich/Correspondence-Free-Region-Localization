#pragma once

#include <libs/optimization_lib/include/objective_functions/objective_function.h>

class ObjectiveAreaPreserving : public ObjectiveFunction
{	
private:
	// mesh vertices and faces
	MatrixX3i F;
	MatrixXd V;

public:
	ObjectiveAreaPreserving();

	virtual void init() override;
	virtual void updateX(const VectorXd& X) override;
	virtual double value() override;
	virtual void gradient(VectorXd& g) override;
	virtual void hessian() override;
	virtual void prepare_hessian() override;
	bool updateJ(const VectorXd& X);
	void setVF(MatrixXd& V, MatrixX3i& F);

	// Jacobian of the parameterization per face
	VectorXd a;
	VectorXd b;
	VectorXd c;
	VectorXd d;

	// Jacobian determinant (ad-bc)
	VectorXd detJ;
	MatrixXd grad;
	vector<Matrix<double, 6, 6>> Hessian;

	//Energy parts - distortion
	VectorXd Area;
	Matrix3Xd D1d, D2d;		//dense mesh derivative matrices

};