#pragma once

#include <bitset>

#include "audio.hpp"
#include "d3d11_renderer_impl.hpp"
#include "mesh.hpp"
#include "XTime.hpp"
#include "view.hpp"

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
		
		float left_click_time = 0.f;
		float right_click_time = 0.f;
		int menu_position = 0;
		
		impl_t* p_impl = nullptr;
		XTime timer;
		entities entities;
		view_t view;

		void init_app(void* window_handle);

		void update();
		void render();
		void set_bitmap(std::bitset<256>& bitmap);

		// timing variables
		double time_elapsed;

		game_manager();
		~game_manager();

		// game state data functions
		// todo::the method body for these functions go in game_manager.cpp
		void new_game() {}
		void load_mesh_data() {}
		void unload_game() {}
		void render_game() {}
		bool l_click_update() { return false; }
		bool r_click_update() { return false; }
		void cleanup() {}
		// updates room metadata such as oxygen remaining, pressure, etc
		void room_updates(float dt);


		// ImGui draw calls
		void imgui_on_draw();

		void draw_main_menu(ImVec2 wh);
		void draw_pause_button(ImVec2 wh);
		void draw_pause_menu(ImVec2 wh);
		void draw_control_message(ImVec2 wh);
		void draw_game_over(ImVec2 wh);
		void draw_oxygen_remaining(ImVec2 wh);


		// various helper functions

		// draws some imgui text in the center of the current window
		void imgui_centertext(std::string text, float scale, ImVec2 wh);

		// gets the size of the game window
		ImVec2 get_window_size();

	private:
		std::bitset<256> bmap;

	};

}

