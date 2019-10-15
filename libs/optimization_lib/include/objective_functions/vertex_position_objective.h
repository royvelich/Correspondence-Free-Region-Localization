#pragma once
#ifndef OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H
#define OPTIMIZATION_LIB_VERTEX_POSITION_OBJECTIVE_H

// STL includes
#include <vector>

// Eigen includes
#include <Eigen/Core>

// Optimization lib includes
#include "./position_objective.h"

template<Eigen::StorageOptions StorageOrder>
class VertexPositionObjective : public PositionObjective<StorageOrder>
{
public:
	/**
	 * Constructors and destructor
	 */
	VertexPositionObjective(const std::shared_ptr<ObjectiveFunctionDataProvider>& objective_function_data_provider, const std::vector<std::pair<int64_t, Eigen::Vector2d>>& index_vertex_pairs) :
		PositionObjective(objective_function_data_provider, index_vertex_pairs.size(), "Vertex Position Objective"),
		index_vertex_pairs_(index_vertex_pairs)
	{
		X_current_.resize(this->objective_vertices_count_, 2);
		X_objective_.resize(this->objective_vertices_count_, 2);
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			X_objective_.row(i) = index_vertex_pairs[i].second;
		}
		this->Initialize();
	}

	virtual ~VertexPositionObjective()
	{

	}

	void OffsetPositionConstraint(const Eigen::Vector2d& offset) override
	{
		X_objective_ = X_objective_.rowwise() + offset.transpose();
	}

private:
	/**
	 * Overrides
	 */

	void CalculateValue(double& f, Eigen::VectorXd& f_per_vertex) override
	{
		f = X_diff_.squaredNorm();
	}

	void CalculateGradient(Eigen::VectorXd& g) override
	{
		g.setZero();
		for (uint64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			const auto current_index = index_vertex_pairs_[i].first;
			g(current_index) = 2.0 * X_diff_(i, 0);
			g(current_index + this->image_vertices_count_) = 2.0 * X_diff_(i, 1);
		}
	}

	void InitializeTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		triplets.resize(this->objective_variables_count_);
		int64_t current_index;
		int64_t current_index_shifted;
		for (uint64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			current_index = index_vertex_pairs_[i].first;
			current_index_shifted = index_vertex_pairs_[i].first + this->image_vertices_count_;
			triplets[i] = Eigen::Triplet<double>(current_index, current_index, 0);
			triplets[i + this->objective_vertices_count_] = Eigen::Triplet<double>(current_index_shifted, current_index_shifted, 0);
		}
	}

	void CalculateTriplets(std::vector<Eigen::Triplet<double>>& triplets) override
	{
		for (uint64_t i = 0; i < this->objective_variables_count_; i++)
		{
			const_cast<double&>(triplets[i].value()) = 2.0;
		}
	}

	void PreUpdate(const Eigen::VectorXd& x) override
	{
		auto X = Eigen::Map<const Eigen::MatrixX2d>(x.data(), x.rows() >> 1, 2);
		for (int64_t i = 0; i < this->objective_vertices_count_; i++)
		{
			X_current_.row(i) = X.row(this->index_vertex_pairs_[i].first);
		}
		X_diff_ = X_current_ - X_objective_;
	}

	/**
	 * Fields
	 */
	std::vector<std::pair<int64_t, Eigen::Vector2d>> index_vertex_pairs_;
	Eigen::MatrixX2d X_objective_;
	Eigen::MatrixX2d X_current_;
	Eigen::MatrixX2d X_diff_;
};

#endif