#pragma once
#ifndef OPTIMIZATION_LIB_DATA_PROVIDER_H
#define OPTIMIZATION_LIB_DATA_PROVIDER_H

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include "./mesh_data_provider.h"

class DataProvider
{
public:
	/**
	 * Constructors and destructor
	 */
	DataProvider(const std::shared_ptr<MeshDataProvider>& mesh_data_provider);
	virtual ~DataProvider();

	/**
	 * Public methods
	 */
	virtual void Update(const Eigen::VectorXd& x) = 0;
	const MeshDataProvider& GetMeshDataProvider() const;

protected:
	/**
	 * Protected Fields
	 */

	// Mesh data provider
	std::shared_ptr<MeshDataProvider> mesh_data_provider_;
};

#endif