#pragma once

#include "d3d11_renderer_impl.hpp"
#include "mesh.hpp"
#include "XTime.h"
#include "view.hpp"
#include <bitset>

namespace crow {
	//using native_handle_type = void*;

	class game_manager
	{
	public:
		//end::renderer_t* renderer = nullptr;
		
		impl_t* p_impl = nullptr;
		XTime timer;
		entities entities;
		view_t view;

		void init_app(void* window_handle);

		void update();
		void render();
		void set_bitmap(std::bitset<256>& bitmap);

		game_manager();
		~game_manager();

	private:
		std::bitset<256> bmap;

	};

}

