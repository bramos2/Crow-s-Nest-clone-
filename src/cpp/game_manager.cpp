#include "../hpp/game_manager.hpp"


namespace crow {
	void game_manager::init_app(void* window_handle)
	{
		audio::initialize();
		p_impl = new impl_t(window_handle, view);
		timer.Restart();
		time_elapsed = 0;

		entities.allocate(2);
		for (size_t i = 0; i < entities.current_size; i++)
		{
			entities.init_entity(i);
		}

		mesh_a temp;
		entities.a_meshes[1] = new mesh_a();
		load_bin_data("res/meshes/Run.bin", temp);
		load_bin_data("res/meshes/Run.bin", *entities.a_meshes[1]);
		std::vector<std::string> paths;
		std::vector<material_a> mats;
		load_mat_data("res/textures/Run.mat", paths, mats);
		//anim_clip animc;
		load_anim_data("res/animations/Run.anim", anim1);
		invert_bind_pose(anim1);
		d = anim1.frames[1].time;


		entities.s_meshes[0] = new mesh_s(clip_mesh(temp));
		//entities.s_meshes[1] = new mesh_s(clip_mesh(temp));

		for (size_t i = 0; i < 1; i++)
		{
			p_impl->create_vertex_buffer(entities.vertex_buffer[i], entities.index_buffer[i], *entities.s_meshes[i]);
			p_impl->create_text_sresources(paths, entities, i);
		}

		p_impl->create_vertex_buffer(entities.vertex_buffer[1], entities.index_buffer[1], *entities.a_meshes[1]);
		p_impl->create_text_sresources(paths, entities, 1);


		DirectX::XMFLOAT4X4 w1;
		DirectX::XMStoreFloat4x4(&w1, entities.world_matrix[0]);
		w1.m[3][0] = -5.f;
		entities.world_matrix[0] = DirectX::XMLoadFloat4x4(&w1);
		int stop = 0;
	}

	void game_manager::update()
	{
		timer.Signal();
		time_elapsed += timer.Delta();
		d += static_cast<float>(timer.Delta());
		if (d >= anim1.duration) {
			d = anim1.frames[1].time;
		}

		key_frame kf = get_tween_frame(anim1, d);
		mult_invbp_tframe(anim1, kf, entities.framexbind[1]);

		p_impl->update(static_cast<float>(timer.Delta()));

	}

	void game_manager::render()
	{
		p_impl->set_render_target_view();



		p_impl->draw_debug_lines(view);
		//p_impl->draw_mesh(view);
		std::vector<uint32_t> temp_inds;
		temp_inds.push_back(0);
		temp_inds.push_back(1);
		p_impl->draw_entities(entities, temp_inds, view);

		// second last thing before presenting screen is all ImGui draw calls
		imgui_on_draw();

		// finally, present the screen
		p_impl->present(1);
	}

	void game_manager::set_bitmap(std::bitset<256>& bitmap)
	{
		bmap = bitmap;
	}

	game_manager::game_manager()
	{

	}

	game_manager::~game_manager()
	{
		audio::cleanup();
		delete p_impl;
	}

	void game_manager::imgui_centertext(std::string text, float scale, ImVec2 wh) {
		ImGui::SetWindowFontScale(wh.x / 960.f * scale);
		float text_size = ImGui::GetFontSize() * text.size() / 2;
		ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
						(text_size / 2.0f));
		ImGui::Text(text.c_str());
	}


	ImVec2 game_manager::get_window_size() {
		RECT r;
		GetClientRect(p_impl->hwnd, &r);

		return ImVec2((float)(r.right - r.left), (float)(r.bottom - r.top));
	}
}