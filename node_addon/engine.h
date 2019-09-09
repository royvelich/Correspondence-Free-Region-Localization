#pragma once
#ifndef ENGINE_H
#define ENGINE_H

// Node API Includes
#include <napi.h>

// STL Includes
#include <memory>

// Eigen Includes
#include <Eigen/Core>

// Optimization Lib Includes
#include <libs/optimization_lib/include/utils/mesh_wrapper.h>

class Engine : public Napi::ObjectWrap<Engine> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Engine(const Napi::CallbackInfo& info);

private:
	enum class ModelFileType
	{
		OBJ,
		OFF,
		UNKNOWN
	};

	static Napi::FunctionReference constructor;

	/**
	 * NAPI Instance Methods
	 */
	Napi::Value LoadModel(const Napi::CallbackInfo& info);
	Napi::Value GetModelVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSoupVertices(const Napi::CallbackInfo& info);
	Napi::Value GetModelFaces(const Napi::CallbackInfo& info);
	Napi::Value GetSoupFaces(const Napi::CallbackInfo& info);
	Napi::Value GetModelBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSoupBufferedVertices(const Napi::CallbackInfo& info);
	Napi::Value GetModelBufferedMeshVertices(const Napi::CallbackInfo& info);
	Napi::Value GetSoupBufferedMeshVertices(const Napi::CallbackInfo& info);

	/**
	 * Regular Instance Methods
	 */
	ModelFileType GetModelFileType(std::string filename);
	Napi::Array CreateFaces(Napi::Env env, const Eigen::MatrixX3i& F);

	/**
	 * Regular Instance Methods (templates)
	 */
	template <typename Derived>
	Napi::Array CreateVertices(Napi::Env env, const Eigen::MatrixBase<Derived>& V)
	{
		Napi::Array vertices_array = Napi::Array::New(env);
		auto entries_per_vertex = V.cols();
		for (int vertex_index = 0; vertex_index < V.rows(); vertex_index++)
		{
			Napi::Object vertex_object = Napi::Object::New(env);
			float x = V(vertex_index, 0);
			float y = V(vertex_index, 1);

			vertex_object.Set("x", x);
			vertex_object.Set("y", y);

			if (entries_per_vertex == 3)
			{
				float z = V(vertex_index, 2);
				vertex_object.Set("z", z);
			}
		
			vertices_array[vertex_index] = vertex_object;
		}

		return vertices_array;
	}

	template <typename Derived>
	Napi::Array CreateBufferedVerticesArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V)
	{
		Napi::Array buffered_vertices_array = Napi::Array::New(env);
		auto entries_per_vertex = V.cols();
		for (int vertex_index = 0; vertex_index < V.rows(); vertex_index++)
		{
			float x = V(vertex_index, 0);
			float y = V(vertex_index, 1);
			float z = entries_per_vertex == 2 ? 0 : V(vertex_index, 2);

			int base_index = 3 * vertex_index;
			buffered_vertices_array[base_index] = x;
			buffered_vertices_array[base_index + 1] = y;
			buffered_vertices_array[base_index + 2] = z;
		}

		return buffered_vertices_array;
	}

	template <typename Derived>
	Napi::Array CreateBufferedMeshVerticesArray(Napi::Env env, const Eigen::MatrixBase<Derived>& V, const Eigen::MatrixXi& F)
	{
		Napi::Array buffered_mesh_vertices_array = Napi::Array::New(env);
		auto entries_per_vertex = V.cols();
		auto entries_per_face = 9;
		for (int face_index = 0; face_index < F.rows(); face_index++)
		{
			int base_index = entries_per_face * face_index;
			for (int i = 0; i < 3; i++)
			{
				int vertex_index = F(face_index, i);
				float x = V(vertex_index, 0);
				float y = V(vertex_index, 1);
				float z = entries_per_vertex == 2 ? 0 : V(vertex_index, 2);
				
				int base_vertex_index = base_index + 3 * i;
				buffered_mesh_vertices_array[base_vertex_index] = x;
				buffered_mesh_vertices_array[base_vertex_index + 1] = y;
				buffered_mesh_vertices_array[base_vertex_index + 2] = z;
			}
		}

		return buffered_mesh_vertices_array;
	}

	/**
	 * Fields
	 */
	MeshWrapper mesh_wrapper_;
};

#endif