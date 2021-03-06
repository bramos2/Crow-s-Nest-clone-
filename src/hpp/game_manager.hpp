#pragma once

#include <fstream>

#include "audio.hpp"
#include "behavior_tree.hpp"
#include "credits.hpp"
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
			GAME_WIN,
			LEVEL_WIN,
			S_SPLASH_FS,
			S_SPLASH_GD,
			S_SPLASH_LV
		} current_state = game_state::MAIN_MENU;

		game_state prev_state = game_state::MAIN_MENU;
		float state_time = 0;
		float enemy_appear_sound_cooldown = 0;
		const float enemy_appear_sound_max_cooldown = 10;

		// animatiors
		animator player_animator;
		animator ai_animator;

		bool debug_mode = 0;
		bool pressing_key = false;

		bool good_ending = false;
		float self_destruct_timer = -1;
		int menu_position = 0;
		int level_number = 1;
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

		// set to true the first time the enemy appears. false otherwise
		// used only by the tutorial
		bool enemy_first_appearance = false;
		// similar to above
		bool first_pressure_console_break = false;

		// camera values
		float3e cam_pos = float3e(0.f, 20.f, -2.f);
		float3e cam_rotation = float3e(-85.f, 0.f, 0.f);

		// list of every single drawable mesh that is currently loaded
		std::vector<mesh_info> all_meshes;

		// a list of every texture in our program
		std::vector<ID3D11ShaderResourceView*> textures;

		// a list of every animatior each containing all the animations respective to their model
		std::vector<animator> animators;
		float shadow_spawn_timer = 0.f;

		struct mesh_types {
			enum {
				PLAYER = 0,
				AI,
				CUBE,
				DOOR,
				EXIT_LIGHT,
				CONSOLE1,
				CONSOLE2,
				BED1,
				CHAIR1,
				ELECTRIC_BOX1,
				SERVER_BOX1,
				SOFA1,
				SOFA2,
				SOFA3,
				BARREL1,
				BARREL2,
				BARREL3,
				CRATE1,
				CRATE2,
				CRATE3,
				LIGHT_BOX,
				DESK1,
				DESK2,
				DESK3,
				CONSOLE3,
				DISK,
				AI_1,
				AI_2,
				AI_3,
				COUNT
			};
		};

		struct texture_list {
			enum {
				PLAYER = 0,
				AI,
				FLOOR0,
				FLOOR1,
				FLOOR2,
				FLOOR2_2, // can also use this in floor 1
				FLOOR4,
				FLOOR5,
				FLOOR5_2,
				FLOOR5_ENTRANCE,
				FLOOR5_SD,
				FLOOR_CARGO,
				WALL1,
				WALL2,
				WALL4,
				DOOR_OPEN,
				DOOR_CLOSED,
				DOOR_EXIT,
				DOOR_BROKEN,
				EXIT_LIGHT_D,
				EXIT_LIGHT_S,
				CONSOLE1_D,
				CONSOLE1_D_PURPLE,
				CONSOLE1_D_RED,
				CONSOLE1_S,
				CONSOLE2,
				BED1,
				CHAIR1,
				ELECTRIC_BOX1,
				SERVER_BOX1,
				SOFA1,
				SOFA2,
				SOFA3,
				CRATE_BARREL,
				CRATE_BARREL_E,
				LIGHT_BOX,
				DESK12,
				DESK3,
				CONSOLE3,
				CONSOLE3_RED,
				CONSOLE3_E,
				SHADOW,
				GUI_SELECT,
				GUI_SELECT2,
				GUI_PAUSE,
				GUI_LOGO,
				EFFECT_STEAM,
				EFFECT_DUST,
				EFFECT_HIT1,
				SPLASH_FS,
				SPLASH_GD,
				SPLASH_LV,
				NPC,
				COUNT
			};
		};

		struct animator_list {
			enum {
				PLAYER = 0,
				AI,
				EXIT_LIGHT,
				AI_1,
				AI_2,
				AI_3,
				COUNT
			};
		};

		// used to display messages after a delay
		struct buffered_message {
			// message to display
			message b_message;
			// time to wait before displaying the message
			// negative means don't display.
			double wait = -1.0;
			// function to be called when the message is displayed
			void (crow::game_manager::* _exit_function)() = nullptr;

			void reset() {
				wait = -1.0;
				_exit_function = nullptr;
			}

			void set(message _m, double _w, void (crow::game_manager::* _e)()) {
				b_message = _m;
				wait = _w;
				_exit_function = _e;
			}
		} c_buffered_message;

		// all ai components
		crow::behavior_tree ai_bt;
		crow::ai_manager ai_m;

		// note we will need a second tree that has different behaviors
		crow::behavior_tree ai_bt2;
		// we will store the rest of the ai into this vector instead for simplicity
		std::vector<crow::ai_manager> ai_managers;
		crow::player_behavior_data player_data;

		// player and ai interactibles will be stored here. their index will be used by the rooms to determine their current location
		std::vector<crow::interactible*> live_entities_inter;

		crow::interactible ai1_it;
		crow::interactible ai2_it;
		crow::interactible ai3_it;

		struct ai_interactibles {
			enum {
				PLAYER = 0,
				AI_1,
				AI_2,
				AI_3,
				COUNT
			};
		};

		// particle stuffs
		std::vector<crow::emitter_sp> emitters;

		// s_bin = filepath to model file to load (mandatory)
		// s_mat = filepath to mat file to load (optional)
		// s_anim = filepath to anim file to load (optional)
		// if s_anim is provided, the model will be loaded as an animated mesh
		// pass in "" for either s_mat or s_anim if they aren't to be used
		void load_mesh_data(std::string filename, mesh_a& temp, int index);
		void load_all_meshes();
		void load_texture_data();
		void load_animation_data();
		void init_app(void* window_handle);

		void change_room_tex();

		// all update functions
		void update();
		void poll_controls(double dt);
		void update_animations(double dt);
		void update_particles(double dt);
		bool l_click_update();
		bool r_click_update();
		bool enemy_and_player_in_same_room();
		// gives you the index of the room with the worker in it
		int room_with_worker();
		// ditto but for the killer dude thingy
		int room_with_enemy();
		// updates room metadata such as oxygen remaining, pressure, doors, etc
		void room_updates(double dt);
		// updates various things related to sound
		void sound_updates(double dt);

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
		void draw_move_pos(ImVec2 wh);
		void draw_pause_button(ImVec2 wh);
		void draw_pause_menu(ImVec2 wh);
		void draw_control_message(ImVec2 wh);
		void draw_game_over(ImVec2 wh);
		void draw_options_menu(ImVec2 wh);
		void draw_oxygen_remaining(ImVec2 wh);
		void draw_pressure_remaining(ImVec2 wh);
		void draw_sd_timer(ImVec2 wh);
		void draw_splash(ImVec2 wh);
		void draw_credits(ImVec2 wh);
		void draw_level_win_screen(ImVec2 wh);
		void draw_game_win_screen(ImVec2 wh);

		// imgui sucks
		ImVec2 imgui_wsize;

		// draws some imgui text in the center of the current window
		void imgui_centertext(std::string text, float scale, ImVec2 wh);

		// gets the size of the game window
		ImVec2 get_window_size();

		// tutorial functions
		void t_first_control_message();
		void t_second_control_message();
		void t_third_control_message();
		void t_fourth_control_message();
		void t_fifth_control_message();
		void t_locked_door_message();
		void t_scrollwheel_message();
		void t_pressure_message2();

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

