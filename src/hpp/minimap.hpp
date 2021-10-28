#pragma once
#include <imgui.h>

#include "../hpp/math_types.hpp"
#include "../hpp/map.hpp"
#include "../hpp/view.hpp"

namespace crow {
	class game_manager;

	struct minimap {
		// the minimun coordinates of the map
		float2e map_minc = {0, 0};
		// the maximun coordinates of the map
		float2e map_maxc = {0, 0};
		// draw scale of objects in the minimap
		float2e scale = {0, 0};
		// the ratio for the x,y coordinates of the min point of the gui on the screen
		// (top left)
		float2e screen_minr = {0, 0};
		//  the ratio for the x,y coordinates of the max point of the gui on the
		//  screen (bottom right)
		float2e screen_maxr = {0, 0};

		float2e resolution = {0, 0};
		float2e window_pos2 = {0, 0};
		float2e window_pos = {0, 0};
		float2e window_ext = {0, 0};

		crow::level* current_level = nullptr;

		float2e minimap_center_position = {0, 0};

		// offset at which rooms will be apart from each other in minimap
		float offset = 5.f;
		// the dimmensions at which the room buttons will be desplayed
		float r_width = 45.f;
		float r_height = 45.f;

		// how zoomed in the minimap is
		float zoom = 3;

		//void load_room();
		void draw_call(game_manager& state);

		minimap();
		minimap(float2e _min, float2e _max);
		void set_window_size(ImVec2 window_size);

		// updates map_minc and map_maxc based on the rooms inside the minimap.
		// additionally centers the minimap unless specified otherwise
		void calculate_extents(bool recenter = true);

		auto inside_minimap(float2e& mouse_pos) -> bool;

		//private:
		bool is_dragging = false;

		// position of the mouse when you click
		// if the mouse hasn't been clicked, this = {-1, -1}
		// if the mouse has been clicked, ranges {0:1, 0:1}
		float2e mouse_position = {0, 0};

		void set_rooms_pos();

		void calculate_mouse_position(float2e& mouse_pos);

		void calculate_mouse_drag(float2e& mouse_pos);

		auto room_off_view(crow::room const& room) const -> bool;

		void reset_state();

	};
}  // namespace crow
