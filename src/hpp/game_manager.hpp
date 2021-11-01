#pragma once

#include <fstream>

#include "audio.hpp"
#include "behavior_tree.hpp"
#include "d3d11_renderer_impl.hpp"
#include "mesh.hpp"
#include "XTime.hpp"
#include "view.hpp"
#include "minimap.hpp"
#include "map.hpp"
#include "player_behavior.hpp"

namespace crow {
	//using native_handle_type = void*;

	class game_manager
	{
	public:
		//end::renderer_t* renderer = nullptr;

		// 
		enum class game_state {
			MAIN_MENU = 0,
			PLAYING = 1,
			PAUSED = 2,
			SETTINGS = 3,
			CREDITS = 4,
			LOADING = 5,
			EXIT = 6,
			GAME_OVER,
			GAME_OVER_PRE,
			GAME_WIN
		} current_state = game_state::MAIN_MENU;

		game_state prev_state = game_state::MAIN_MENU;
		float state_time = 0;

		// animatiors
		animator player_animator;
		animator ai_animator;

		bool debug_mode = 0;

		float left_click_time = 0.f;
		//float right_click_time = 0.f;
		int menu_position = 0;
		int level_number = 0;
		int mwheel_delta = 0;
		float2e mouse_pos;
		// use this for any imgui functions that may for some reason need the mouse pos
		float2e mouse_pos_gui;
		
		impl_t* p_impl = nullptr;
		XTime timer;
		entities entities;
		view_t view;
		level current_level;
		crow::minimap minimap;
		crow::message current_message;

		// camera values
		float3e cam_pos = float3e(0.f, 20.f, -2.f);
		float3e cam_rotation = float3e(-85.f, 0.f, 0.f);

		// list of every single drawable mesh that is currently loaded
		std::vector<mesh_info> all_meshes;

		// a list of every texture in our program
		std::vector<ID3D11ShaderResourceView*> textures;

		// a list of every animatior each containing all the animations respective to their model
		std::vector<animator> animators;

		struct mesh_types {
			enum {
				PLAYER = 0,
				AI,
				CUBE,
				DOOR,
				EXIT_LIGHT,
				CONSOLE1,
				CONSOLE2,
				COUNT
			};
		};

		struct texture_list {
			enum {
				PLAYER = 0,
				AI,
				FLOOR1,
				WALL1,
				DOOR_OPEN,
				DOOR_CLOSED,
				EXIT_LIGHT_D,
				EXIT_LIGHT_S,
				CONSOLE1_D,
				CONSOLE1_S,
				CONSOLE2,
				COUNT
			};
		};

		struct animator_list {
			enum {
				PLAYER = 0,
				AI,
				EXIT_LIGHT,
				COUNT
			};
		};

		// all ai components
		crow::behavior_tree ai_bt;
		crow::ai_manager ai_m;
		crow::player_behavior_data player_data;

		// s_bin = filepath to model file to load (mandatory)
		// s_mat = filepath to mat file to load (optional)
		// s_anim = filepath to anim file to load (optional)
		// if s_anim is provided, the model will be loaded as an animated mesh
		// pass in "" for either s_mat or s_anim if they aren't to be used
		void load_mesh_data(std::string s_bin, std::string s_mat, std::string s_anim, int index);
		void load_mesh_data(std::string filename, int index);
		void load_all_meshes();
		void load_texture_data();
		void load_animation_data();
		void init_app(void* window_handle);

		// all update functions
		void update();
		void poll_controls(double dt);
		void update_animations(double dt);
		bool l_click_update();
		bool r_click_update();
		// updates room metadata such as oxygen remaining, pressure, etc
		void room_updates(double dt);

		void render();
		void render_game();

		// timing variables
		double time_elapsed = 0;

		game_manager();
		~game_manager();

		// game state data functions
		void save_game();
		void load_game();
		void new_game();
		void end_game();
		void load_level(int lv);
		void change_level(int lv);
		void unload_level();
		// call this whenever the worker dies to transition to gameover state
		void game_over();
		void cleanup();

		// ImGui draw calls
		void imgui_on_draw();

		void draw_main_menu(ImVec2 wh);
		void draw_pause_button(ImVec2 wh);
		void draw_pause_menu(ImVec2 wh);
		void draw_control_message(ImVec2 wh);
		void draw_game_over(ImVec2 wh);
		void draw_options_menu(ImVec2 wh);
		void draw_oxygen_remaining(ImVec2 wh);
		void draw_pressure_remaining(ImVec2 wh);

		// imgui sucks
		ImVec2 imgui_wsize;

		// various helper functions

		// draws some imgui text in the center of the current window
		void imgui_centertext(std::string text, float scale, ImVec2 wh);

		// gets the size of the game window
		ImVec2 get_window_size();

	private:
		std::vector<double> buttons;
		std::vector<unsigned int> buttons_frame;
		const int button_mappings[2] = {
			VK_LBUTTON, VK_RBUTTON
		};

		enum controls {
			l_mouse = 0,
			r_mouse = 1
		};
	};

}

