#include "../hpp/game_manager.hpp"


namespace crow {
	void game_manager::load_mesh_data(std::string s_bin, std::string s_mat, std::string s_anim, int index) {
		mesh_a temp;
		LoadBinData(s_bin.c_str(), temp);
		std::vector<std::string> paths;
		std::vector<material_a> mats;
		LoadMatData(s_mat.c_str(), paths, mats);
		animClip animc;
		LoadAnimData(s_anim.c_str(), animc);

		all_meshes.resize(1);
		all_meshes[0].s_mesh = new mesh_s(clip_a_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[0].vertex_buffer, all_meshes[0].index_buffer, *all_meshes[0].s_mesh);
		p_impl->create_text_sresources(paths, all_meshes[0]);
	}

	void game_manager::init_app(void* window_handle)
	{
		audio::initialize();
		p_impl = new impl_t(window_handle, view);
		timer.Restart();
		time_elapsed = 0;

		all_meshes.resize(1);
		load_mesh_data("Assets/Run.bin", "Assets/Run.mat", "Assets/Run.anim", 0);

		entities.allocate(2);
		for (size_t i = 0; i < entities.current_size; i++)
		{
			entities.init_entity(i);
			entities.mesh_ptrs[i] = &all_meshes[0];
		}

		// WARNING: DANGER
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
		// WARNING: DANGER


		DirectX::XMFLOAT4X4 w1;
		DirectX::XMStoreFloat4x4(&w1, entities.world_matrix[0]);
		w1.m[3][0] = -5.f;
		entities.world_matrix[0] = DirectX::XMLoadFloat4x4(&w1);
		int stop = 0;

		load_level(0);
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
		// WARNING: DANGER!

		timer.Signal(); double dt = timer.Delta();
		time_elapsed += dt;
		p_impl->update(static_cast<float>(dt));

		// capture mouse position
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(p_impl->hwnd, &p);
		mouse_pos.x = (float)p.x;
		mouse_pos.y = (float)p.y;

		// debug mode toggle, allows debug camera
		if (!debug_mode && (GetKeyState(VK_F1))) {
			printf("\nDEBUG MODE ENABLED\n");
			debug_mode = true;
		}

		// debug mode updates
		if (debug_mode) {
			float4e translate = {0, 0, 0, 0};

			// debug camera controls
			/* WASD = basic movement                                                     */
			/* ARROW KEYS = rotate camera                                                */
			/* Z/X = barrel roll camera                                                  */
			/* LSHIFT/SPACE = move up/down                                               */
			/* RSHIFT (hold) = don't apply rotation while moving                         */
			/* R = reset camera to room's initial camera                                 */

			// movement for camera
			if ((GetKeyState('W'      ) & 0x8000) != 0) translate.z += view.debug_camera_mspeed * dt;
			if ((GetKeyState('S'      ) & 0x8000) != 0) translate.z -= view.debug_camera_mspeed * dt;
			if ((GetKeyState('A'      ) & 0x8000) != 0) translate.x -= view.debug_camera_mspeed * dt;
			if ((GetKeyState('D'      ) & 0x8000) != 0) translate.x += view.debug_camera_mspeed * dt;
			if ((GetKeyState(VK_LSHIFT) & 0x8000) != 0) translate.y -= view.debug_camera_mspeed * dt;
			if ((GetKeyState(VK_SPACE ) & 0x8000) != 0) translate.y += view.debug_camera_mspeed * dt;
			
			// rotation for camera
			if ((GetKeyState(VK_UP    ) & 0x8000) != 0) view.rotation.x += view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_DOWN  ) & 0x8000) != 0) view.rotation.x -= view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_LEFT  ) & 0x8000) != 0) view.rotation.y += view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_RIGHT ) & 0x8000) != 0) view.rotation.y -= view.debug_camera_rspeed * dt;
			if ((GetKeyState('X'      ) & 0x8000) != 0) view.rotation.z += view.debug_camera_rspeed * dt;
			if ((GetKeyState('Z'      ) & 0x8000) != 0) view.rotation.z -= view.debug_camera_rspeed * dt;

			// apply rotation to movement
			view.update_rotation_matrix();
			// but not if rshift is held
			if ((GetKeyState(VK_RSHIFT ) & 0x8000) == 0) translate = MatrixVectorMult(translate, view.rot_mat);

			// update the position
			view.position.x += translate.x;
			view.position.y += translate.y;
			view.position.z += translate.z;


			if ((GetKeyState('R') & 0x8000) != 0 && current_level.selected_room) crow::update_room_cam(current_level.selected_room, view);
			
			// VERY LAST thing to do should be to update the camera
			view.update();
		}
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

	void game_manager::unload_all_meshes() {
		for (int i = 0; i < all_meshes.size(); i++) {
			delete all_meshes[i].framexbind;
			delete all_meshes[i].a_mesh;
			delete all_meshes[i].s_mesh;
			all_meshes[i].vertex_buffer->Release();
			all_meshes[i].index_buffer->Release();
			if (all_meshes[i].s_resource_view != nullptr) all_meshes[i].s_resource_view->Release();
			if (all_meshes[i].emissive != nullptr)        all_meshes[i].emissive->Release();
			if (all_meshes[i].specular != nullptr)        all_meshes[i].specular->Release();

			all_meshes[i].framexbind = nullptr;
			all_meshes[i].a_mesh = nullptr;
			all_meshes[i].s_mesh = nullptr;
			all_meshes[i].vertex_buffer = nullptr;
			all_meshes[i].index_buffer = nullptr;
			all_meshes[i].s_resource_view = nullptr;
			all_meshes[i].emissive = nullptr;
			all_meshes[i].specular = nullptr;
		}
	}

	game_manager::game_manager()
	{

	}

	game_manager::~game_manager()
	{
		unload_all_meshes();
		audio::cleanup();
		delete p_impl;
	}

	

	void game_manager::change_level(int lv) {
		// TODO::clean the level

		// after all clean up is done, it's time to start loading the next level
		if (lv - 1 == crow::final_level) {
			// we beat the game, so send us to the endgame sequence, whatever that may
			// be

			// todo::this
		} else {
			// load the next level
			load_level(lv);
		}
	}

	void game_manager::load_level(int lv) {
		current_level.load_level(this, lv);
		// todo::call the load_entities method somehow (this must be before the lines
		// that load the camera)

		// auto-load the first room
		current_level.select_default_room();
		crow::update_room_cam(current_level.selected_room, view);
		current_level.p_inter = &player_data.player_interact;

		// setting up minimap
		minimap = crow::minimap({0.0f, 0.65f}, {0.4f, 0.35f});
		minimap.map_minc = {-300, -300};
		minimap.map_maxc = {300, 300};
		minimap.screen_minr = {0.0f, 0.65f};
		minimap.screen_maxr = {0.25f, 0.35f};
		minimap.resolution = {1920, 1080};
		minimap.set_window_size(get_window_size());
		minimap.current_level = &current_level;
		minimap.calculate_extents();

		// TODO::ai init


		current_state = crow::game_manager::game_state::PLAYING;
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