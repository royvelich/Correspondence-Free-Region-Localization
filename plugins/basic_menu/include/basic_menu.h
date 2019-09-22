#pragma once

#ifndef RDS_PLUGINS_BASIC_MENU_H
#define RDS_PLUGINS_BASIC_MENU_H

#include "menu_utils.h"

class BasicMenu : public ImGuiMenu
{
private:
	//Basic (necessary) parameteres
	int input_view_id, output_view_id;
	MenuUtils::View view;
	MenuUtils::MouseMode mouse_mode;
	MenuUtils::Parametrization param_type;
	float texture_scaling_input, texture_scaling_output;
	Vector3f Highlighted_face_color, Fixed_face_color, Fixed_vertex_color;
	bool Highlighted_face;
	Vector3f model_color, Dragged_face_color, Dragged_vertex_color, Vertex_Energy_color, text_color;
	MatrixXd color_per_face, Vertices_Input, Vertices_output, color_per_vertex;
	set<int> selected_faces, selected_vertices;
	float core_percentage_size;
	bool IsTranslate;
	int Translate_Index, Model_Translate_ID, Core_Translate_ID, down_mouse_x, down_mouse_y;
	ImGuiMenu menu;

	vector<int> *HandlesInd; //pointer to indices in constraitPositional
	MatrixX2d *HandlesPosDeformed; //pointer to positions in constraitPositional

	//Solver Button Parameters
	bool solver_on, solverInitialized, show_text;
	MenuUtils::Distortion distortion_type;
	MenuUtils::SolverType solver_type;
	float Max_Distortion;
	
	// Solver thread
	thread solver_thread;
	shared_ptr<Newton> newton;
	shared_ptr<GradientDescent> gradient_descent;
	shared_ptr<Solver> solver;
	shared_ptr<TotalObjective> totalObjective;

protected:
	//Basic (necessary) parameteres
	map<unsigned int, string> data_id_to_name;

public:
	//Constructor & initialization
	BasicMenu();
	~BasicMenu(){}

	// callbacks
	IGL_INLINE virtual void draw_viewer_menu() override;
	IGL_INLINE virtual void init(Viewer *_viewer) override;
	IGL_INLINE virtual void post_resize(int w, int h) override;
	IGL_INLINE virtual bool mouse_move(int mouse_x, int mouse_y) override;
	IGL_INLINE virtual bool mouse_down(int button, int modifier) override;
	IGL_INLINE virtual bool mouse_up(int button, int modifier) override;
	IGL_INLINE virtual bool pre_draw() override;
	IGL_INLINE virtual void shutdown() override;
	IGL_INLINE virtual bool key_pressed(unsigned int key, int modifiers) override;
			
	//Draw menu methods
	void Draw_menu_for_cores();
	void Draw_menu_for_models();
	void Draw_menu_for_Solver();
	void Draw_menu_for_Energies();
	void Draw_menu_for_colors();
	void Draw_menu_for_text_results();

	//Pick faces & vertices and highlight them
	int pick_face(MatrixXd& V, MatrixXi& F, MenuUtils::View LR);
	int pick_vertex(MatrixXd& V, MatrixXi& F, MenuUtils::View LR);
	void follow_and_mark_selected_faces();
	RowVector3d get_face_avg();
	void UpdateHandles();
	void UpdateEnergyColors();

	//Name
	void set_name_mapping(unsigned int data_id, string name);
	
	//Basic Methods
	int InputModelID();
	ViewerData& InputModel();
	ViewerData& OutputModel();
	int OutputModelID();
	void Update_view();
	void update_mesh();
	void update_texture(MatrixXd& V_uv);

	//Start/Stop the solver Thread
	void start_solver_thread();
	void stop_solver_thread();
	void initializeSolver();

	//FD check
	void checkGradients();
	void checkHessians();
};

#endif