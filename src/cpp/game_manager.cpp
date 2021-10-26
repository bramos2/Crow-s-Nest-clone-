#include "../hpp/game_manager.hpp"


namespace crow {

	// made changes to the loading process
	void game_manager::load_mesh_data(std::string s_bin, std::string s_mat, std::string s_anim, int index) {
		//// load either as static mesh or as animated mesh based on whether or not we have an .anim file
		//if (s_anim.length()) {
		//	all_meshes[index].a_mesh = new mesh_a();
		//	load_bin_data(s_bin.c_str(), *all_meshes[index].a_mesh);

		//	anim_clip animc;
		//	load_anim_data(s_anim.c_str(), animc);
		//	invert_bind_pose(animc);
		//	all_meshes[index].anim = animc;

		//	// create vertex buffer
		//	p_impl->create_vertex_buffer(all_meshes[index].vertex_buffer, all_meshes[index].index_buffer, *all_meshes[index].a_mesh);
		//} else {
		//	// static mesh
		//	mesh_a temp;
		//	load_bin_data(s_bin.c_str(), temp);
		//	all_meshes[index].s_mesh = new mesh_s(clip_mesh(temp));

		//	// create vertex buffer
		//	p_impl->create_vertex_buffer(all_meshes[index].vertex_buffer, all_meshes[index].index_buffer, *all_meshes[index].s_mesh);
		//}
		//
		//// load textures if textures are provided
		//if (s_mat.length()) {
		//	std::vector<std::string> paths;
		//	std::vector<material_a> mats;
		//	load_mat_data(s_mat.c_str(), paths, mats);

		//	// load textures
		//	p_impl->create_text_sresources(paths, all_meshes[index]);
		//}
	}

	void game_manager::load_mesh_data(std::string filename, int index) {
		std::string s_bin = "res/meshes/" + filename + ".bin";
		std::string s_mat = "res/textures/" + filename + ".mat";
		std::string s_anim = "res/animations/" + filename + ".anim";
		load_mesh_data(s_bin, s_mat, s_anim, index);
	}

	void game_manager::load_all_meshes()
	{
		all_meshes.resize(mesh_types::COUNT);

		// loading player mesh
		all_meshes[mesh_types::PLAYER].a_mesh = new mesh_a();
		load_bin_data("res/meshes/guy.bin", *all_meshes[mesh_types::PLAYER].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::PLAYER].vertex_buffer, all_meshes[mesh_types::PLAYER].index_buffer, *all_meshes[mesh_types::PLAYER].a_mesh);

		// loading AI mesh
		all_meshes[mesh_types::AI].a_mesh = new mesh_a();
		load_bin_data("res/meshes/slasher_run.bin", *all_meshes[mesh_types::AI].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::AI].vertex_buffer, all_meshes[mesh_types::AI].index_buffer, *all_meshes[mesh_types::AI].a_mesh);

		// loading texture cube mesh
		mesh_a temp;
		load_bin_data("res/meshes/floor1.bin", temp);
		all_meshes[mesh_types::CUBE].s_mesh = new mesh_s(clip_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[mesh_types::CUBE].vertex_buffer, all_meshes[mesh_types::CUBE].index_buffer, *all_meshes[mesh_types::CUBE].s_mesh);
    
		// loading door mesh
		load_bin_data("res/meshes/door2.bin", temp);
		all_meshes[mesh_types::DOOR].s_mesh = new mesh_s(clip_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[mesh_types::DOOR].vertex_buffer, all_meshes[mesh_types::DOOR].index_buffer, *all_meshes[mesh_types::DOOR].s_mesh);

		// loading exit light mesh
		all_meshes[mesh_types::EXIT_LIGHT].a_mesh = new mesh_a();
		load_bin_data("res/meshes/exit_light.bin", *all_meshes[mesh_types::EXIT_LIGHT].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::EXIT_LIGHT].vertex_buffer, all_meshes[mesh_types::EXIT_LIGHT].index_buffer, *all_meshes[mesh_types::EXIT_LIGHT].a_mesh);

		// loading console1 mesh
		load_bin_data("res/meshes/console1.bin", temp);
		all_meshes[mesh_types::CONSOLE1].s_mesh = new mesh_s(clip_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[mesh_types::CONSOLE1].vertex_buffer, all_meshes[mesh_types::CONSOLE1].index_buffer, *all_meshes[mesh_types::CONSOLE1].s_mesh);

		// loading console2 mesh
		load_bin_data("res/meshes/console2.bin", temp);
		all_meshes[mesh_types::CONSOLE2].s_mesh = new mesh_s(clip_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[mesh_types::CONSOLE2].vertex_buffer, all_meshes[mesh_types::CONSOLE2].index_buffer, *all_meshes[mesh_types::CONSOLE2].s_mesh);
	}

	void game_manager::load_texture_data() {
		textures.resize(texture_list::COUNT);

		p_impl->create_texture("res/textures/player.dds", textures[texture_list::PLAYER]);
		p_impl->create_texture("res/textures/enemy1.dds", textures[texture_list::AI]);
		p_impl->create_texture("res/textures/floor_1.dds", textures[texture_list::FLOOR1]);
		p_impl->create_texture("res/textures/door_open.dds", textures[texture_list::DOOR_OPEN]);
		p_impl->create_texture("res/textures/door_closed.dds", textures[texture_list::DOOR_CLOSED]);
		p_impl->create_texture("res/textures/exit_light_d.dds", textures[texture_list::EXIT_LIGHT_D]);
		p_impl->create_texture("res/textures/exit_light_s.dds", textures[texture_list::EXIT_LIGHT_S]);
		p_impl->create_texture("res/textures/console1_d.dds", textures[texture_list::CONSOLE1_D]);
		p_impl->create_texture("res/textures/console1_s.dds", textures[texture_list::CONSOLE1_S]);
		p_impl->create_texture("res/textures/console2.dds", textures[texture_list::CONSOLE2]);
	}

	void crow::game_manager::load_animation_data()
	{
		animators.resize(animator_list::COUNT);

		size_t i = animator_list::PLAYER;
		animators[i].animations.resize(2);
		load_anim_data("res/animations/guyf.anim", animators[i].animations[0]);
		load_anim_data("res/animations/guy.anim", animators[i].animations[1]);
		get_inverted_bind_pose(animators[i].animations[1].frames[0], animators[i]);

		i = animator_list::AI;
		animators[i].animations.resize(2);
		load_anim_data("res/animations/slasher_run.anim", animators[i].animations[0]);
		load_anim_data("res/animations/slasher_attack.anim", animators[i].animations[1]);
		get_inverted_bind_pose(animators[i].animations[0].frames[0], animators[i]);

		i = animator_list::EXIT_LIGHT;
		animators[i].animations.resize(1);
		load_anim_data("res/animations/exit_light.anim", animators[i].animations[0]);
		get_inverted_bind_pose(animators[i].animations[0].frames[0], animators[i]);
	}

	void game_manager::init_app(void* window_handle)
	{
		// initialize audio
		audio::sound_loaded = false;
		audio::initialize();

		// initialize the renderer
		p_impl = new impl_t(window_handle, view);
		imgui_wsize = get_window_size();


		// initialize the timer
		timer.Restart();
		time_elapsed = 0;

		// initialize the controls
		buttons.resize(2);
		buttons_frame.resize(2);
		for (int i = 0; i < buttons.size(); i++) {
			buttons[i] = 0;
			buttons_frame[i] = 0;
		}

		// load main menu
		current_state = game_state::MAIN_MENU;
	}

	void game_manager::update()
	{
		timer.Signal(); double dt = timer.Delta();
		time_elapsed += dt;
		p_impl->update(static_cast<float>(dt));

		
		// updates that run irregardless of the game state
		current_message.update(dt);
		if (current_level.interacting && current_message.progress_max &&
			current_message.progress_max == current_message.progress) {
		  current_level.interacting->activate();
		  current_level.interacting = nullptr;
		}
		left_click_time += dt;
		right_click_time += dt;

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

		// game state update
		switch (current_state) {
		case game_state::PLAYING:
			// the last thing that happens in update should always be player controls
			poll_controls(dt);
			l_click_update();
			r_click_update();
			
			// all ai updates (player and enemy) here
			if (current_level.found_ai) ai_bt.run(dt);
			crow::path_through(player_data, entities, static_cast<size_t>(crow::entity::WORKER), dt);
			entities.update_transform_data(dt);

			// check for worker alive to end the game if he is dead
			if (!player_data.player_interact.is_active) {
				end_game();
				prev_state = current_state = game_state::GAME_OVER;
				state_time = 0;
				break;
			}

			// all this just to update the angle of the model of the player
			player_data.p_matrix.scale = { 0.25f, 0.25f, 0.25f };
			player_data.p_matrix.rotate_y_axis_from_velocity(entities.velocities[(int)crow::entity::WORKER]);
			player_data.p_matrix.update_position(entities.world_matrix[(int)crow::entity::WORKER]);
			player_data.p_matrix.update();
			entities.world_matrix[(int)crow::entity::WORKER] = player_data.p_matrix.final_matrix;

			// same for the enemy model
			ai_bt.e_matrix.rotate_y_axis_from_velocity(entities.velocities[(int)crow::entity::SPHYNX]);
			ai_bt.e_matrix.update_position(entities.world_matrix[(int)crow::entity::SPHYNX]);
			ai_bt.e_matrix.update();
			entities.world_matrix[(int)crow::entity::SPHYNX] = ai_bt.e_matrix.final_matrix;

			// TODO::room_updates(dt)
			// room_updates(dt);
			audio::update_audio_timers(this, dt);

			// fetch any message that might have been summoned by the player object
			if (current_level.msg.time_remaining > 0) {
				// steal the info
				current_message = current_level.msg;
				// mark as read
				current_level.msg.time_remaining = 0;
				// read: 2:11 PM
			}

			// this should be the last thing that is updated in the state
			update_animations(dt);
			break;
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

		state_time += dt;
		// band-aid for re-setting state_time and prev_state
		if (prev_state != current_state) {
			prev_state = current_state;
			state_time = 0;
		}
	}

	void game_manager::update_animations(double dt) {
		// update all animations for all animated entities
		for (int i = 0; i < entities.current_size; i++) {
			if (entities.mesh_ptrs[i]->animator) {
				entities.mesh_ptrs[i]->animator->update(entities.framexbind[i], dt);
				//// timer increment
				//entities.anim_time[i] += dt;
				//if (entities.anim_time[i] > entities.mesh_ptrs[i]->anim.duration) {
				//	entities.anim_time[i] = entities.mesh_ptrs[i]->anim.frames[1].time;
				//}
				//// update keyframe/bindpose
				//key_frame kf = get_tween_frame(entities.mesh_ptrs[i]->anim, entities.anim_time[i]);
				//mult_invbp_tframe(entities.mesh_ptrs[i]->anim, kf, entities.framexbind[i]);
			}
		}
	}

	
	bool game_manager::l_click_update() {
		if (buttons_frame[controls::l_mouse] != 1) return false;

	  // processing for left clicks while you are currently playing the game
	  if (current_level.selected_room && current_level.selected_room->has_player) {
		// these next two lines prevents the player from moving when you click on
		// the minimap
		if (!minimap.inside_minimap(mouse_pos)) {
		ImVec2 wh = get_window_size();

		  // crow::audio::play_sfx(0);
		  float3e mouse_point = crow::mouse_to_floor(view, mouse_pos, wh.x, wh.y);
		  // y = -1 out of bound
		  if (mouse_point.y != -1) {
			const float3e player_pos = entities.get_world_position(
				static_cast<size_t>(crow::entity::WORKER));
			std::vector<float2e> temporary_results =
				current_level.selected_room->get_path(
					float2e(player_pos.x, player_pos.z),
					float2e(mouse_point.x, mouse_point.z));

			if (temporary_results.size()) {
			  // if the clicked position is the same as the previous position,
			  // then we can assume that you've double clicked. thus, the
			  // worker should run instead of walk
			  if (player_data.path_result.size() &&
				  player_data.path_result[0] == temporary_results[0]) {
				// check to ensure that the clicks were close enough to each
				// other to count as a double click.
				if (left_click_time < 0.5f) {
				  // worker starts running to destination
				  player_data.worker_speed = player_data.worker_run_speed;

				  // plays footstep sound when worker moves
				  crow::audio::add_footstep_sound(
					  (float4x4_a*)&entities.world_matrix[static_cast<size_t>(
						  crow::entity::WORKER)],
					  0.285f);
				}
			  } else {
				// worker starts walking to destination
				player_data.worker_speed = player_data.worker_walk_speed;

				// plays footstep sound when worker moves
				crow::audio::add_footstep_sound(
					  (float4x4_a*)&entities.world_matrix[static_cast<size_t>(
						  crow::entity::WORKER)], 0.5f);
			  }
			}

			// set the worker's path
			player_data.path_result = temporary_results;
			// disable interaction with object
			if (current_level.interacting) {
			  current_level.interacting = nullptr;
			  current_message = message();
			}
		  }
		}
	  }
	  left_click_time = 0;
	  return true;
	}

	bool game_manager::r_click_update() {
		if (buttons_frame[controls::r_mouse] != 1) return false;

		crow::room* selected_room = current_level.selected_room;
		if (selected_room && selected_room->has_player) {
		player_data.interacting = false;
		player_data.target = nullptr;
		ImVec2 wh = get_window_size();

		float3e mouse_point = crow::mouse_to_floor(view, mouse_pos, wh.x, wh.y);
		if (mouse_point.y == -1) {
			return true;
		}
		const crow::tile* clicked_tile =
			selected_room->get_tile_at(float2e(mouse_point.x, mouse_point.z));

		if (!clicked_tile) {
			return true;
		}

		for (auto& i : selected_room->objects) {
			if (clicked_tile->row == i->y && clicked_tile->col == i->x) {
			player_data.interacting = true;
			player_data.target = i;

			const float3e p_pos = entities.get_world_position(
				static_cast<size_t>(crow::entity::WORKER));

			const crow::tile* p_tile =
				selected_room->get_tile_at({p_pos.x, p_pos.z});

			if (!p_tile) {
				return true;
			}

			if (p_tile == clicked_tile) {
				player_data.path_result.clear();
				break;
			}

			float2e adjacent_tile =
				float2e{static_cast<float>(p_tile->col) -
								static_cast<float>(clicked_tile->col),
							static_cast<float>(p_tile->row) -
								static_cast<float>(clicked_tile->row)};

			adjacent_tile = adjacent_tile.normalize();
			for (size_t i = 0; i < 2; i++) {
				if (adjacent_tile[i] >= 0.5f) {
				adjacent_tile[i] = 1.f;
				continue;
				} else if (adjacent_tile[i] <= -0.5f) {
				adjacent_tile[i] = -1.f;
				}
			}

			adjacent_tile = {
				adjacent_tile.x + static_cast<float>(clicked_tile->col),
				adjacent_tile.y + static_cast<float>(clicked_tile->row)};

			adjacent_tile =
				selected_room->get_tile_wpos(static_cast<int>(adjacent_tile.x),
												static_cast<int>(adjacent_tile.y));


			std::vector<float2e> temporary_results =
				selected_room->get_path(float2e(p_pos.x, p_pos.z), adjacent_tile);

			if (!temporary_results.empty() && !player_data.path_result.empty()) {
				// check for double click on same tile
				if (player_data.path_result.size() &&
						player_data.path_result[0] == temporary_results[0]) {
					// check to ensure that the clicks were close enough to each
					// other to count as a double click.
					if (right_click_time < 0.5f) {
						player_data.worker_speed = player_data.worker_run_speed;

						// plays footstep sound when worker moves
						crow::audio::add_footstep_sound(
							(float4x4_a*)&entities.world_matrix[static_cast<size_t>(
								crow::entity::WORKER)],
							0.285f);
					}
				} else {
					player_data.worker_speed = player_data.worker_walk_speed;
				
					crow::audio::add_footstep_sound(
						(float4x4_a*)&entities.world_matrix[static_cast<size_t>(
							crow::entity::WORKER)],
						0.5f);
				}
			}

			// set the worker's path
			player_data.path_result = temporary_results;

			break;
			}
		}
		}

		right_click_time = 0;
		return true;
	}

	void game_manager::render()
	{
		p_impl->set_render_target_view();

		switch (current_state) {
		case game_state::PLAYING: // falling case
		case game_state::PAUSED:
			render_game();
			break;
		}

		// second last thing before presenting screen is all ImGui draw calls
		imgui_on_draw();

		// finally, present the screen
		p_impl->present(1);
	}

	void game_manager::render_game() {
		p_impl->draw_debug_lines(view);
		//p_impl->draw_mesh(view);

		if (current_level.selected_room && entities.current_size > 0) {
			for (size_t i = 0; i < current_level.selected_room->object_indices.size(); ++i) {
				p_impl->draw_entities(entities, current_level.selected_room->object_indices, view);
			}
		}
	}

	void game_manager::cleanup() {
		// delete all meshes and associated data
		for (int i = 0; i < all_meshes.size(); i++) {
			delete all_meshes[i].a_mesh;
			delete all_meshes[i].s_mesh;
			if (all_meshes[i].vertex_buffer != nullptr)   all_meshes[i].vertex_buffer->Release();
			if (all_meshes[i].index_buffer != nullptr)    all_meshes[i].index_buffer->Release();
			/*if (all_meshes[i].s_resource_view != nullptr) all_meshes[i].s_resource_view->Release();
			if (all_meshes[i].emissive != nullptr)        all_meshes[i].emissive->Release();
			if (all_meshes[i].specular != nullptr)        all_meshes[i].specular->Release();*/

			all_meshes[i].a_mesh = nullptr;
			all_meshes[i].s_mesh = nullptr;
			all_meshes[i].vertex_buffer = nullptr;
			all_meshes[i].index_buffer = nullptr;
			/*all_meshes[i].s_resource_view = nullptr;
			all_meshes[i].emissive = nullptr;
			all_meshes[i].specular = nullptr;*/
		}

		// clearing texture data
		for (auto& m : textures) {
			if (m) {
				m->Release();
			}
		}
		textures.clear();


	}

	game_manager::game_manager()
	{

	}

	game_manager::~game_manager()
	{
		cleanup();
		audio::cleanup();
		delete p_impl;
	}

	void game_manager::new_game() {
		// load all the meshes that we are going to need
		//all_meshes.resize(3);
		//load_mesh_data("guy", 0);
		////load_mesh_data("res/meshes/Run.bin", "res/textures/Run.mat", "", 1);
		//load_mesh_data("slasher_run", 1);
		//load_mesh_data("res/meshes/floor1.bin", "res/textures/floor1.mat", "", 2);
		//scale_matrix(entities.world_matrix[2], 0.1f, 0.1f, 0.1f);
		load_texture_data();
		load_all_meshes();
		load_animation_data();

		//textures.resize(1);
		//p_impl->create_imgui_texture("res/textures/gui/go.dds", textures[0]);

		//assigning animatiors, could be done inside function if meshes have been initialized
		all_meshes[mesh_types::PLAYER].animator = &animators[animator_list::PLAYER];
		all_meshes[mesh_types::AI].animator = &animators[animator_list::AI];
		all_meshes[mesh_types::EXIT_LIGHT].animator = &animators[animator_list::EXIT_LIGHT];

		// initialize the first two entities
		entities.allocate_and_init(2);
		entities.mesh_ptrs[0] = &all_meshes[mesh_types::PLAYER];
		entities.s_resource_view[0] = textures[texture_list::PLAYER];

		entities.mesh_ptrs[1] = &all_meshes[mesh_types::AI];
		entities.s_resource_view[1] = textures[texture_list::AI];

		load_level(0);
	}

	void game_manager::end_game() {
		current_level.clean_level();
		cleanup();
		entities.pop_all();
		ai_bt.clean_tree();
	}

	void game_manager::change_level(int lv) {
		// after all clean up is done, it's time to start loading the next level
		if (lv - 1 == crow::final_level) {
			// we beat the game, so send us to the endgame sequence, whatever that may be
			end_game();
			current_state = prev_state = game_state::CREDITS;
			state_time = 0;
		} else {
			// unload the previous level
			unload_level();

			// load the next level
			load_level(lv);
		}
	}

	void game_manager::load_level(int lv) {
		// reset any variables that may need resetting
		player_data.player_interact.is_active = true;

		// load the level data first
		current_level.load_level(this, lv);

		// load additional entities that are in the level
		for (auto& row : current_level.rooms) {
			for (auto& r : row) {
				if (r.id != 0) {
					r.load_entities(*this);
				}
			}
		}

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

		// initialize ai
		ai_m.init_manager(&entities, &current_level);
		ai_bt.aim = &ai_m;
		ai_bt.build_tree();

		current_state = crow::game_manager::game_state::PLAYING;
	}

	void game_manager::unload_level() {
		// remove all entities except for the sphynx and worker
		while (entities.current_size > 2) {
			entities.pop_back();
		}

		// reset the sphynx and worker entity data
		delete entities.framexbind[0];
		delete entities.framexbind[1];
		entities.init_entity(0);
		entities.init_entity(1);
		entities.mesh_ptrs[0] = &all_meshes[0];
		entities.mesh_ptrs[1] = &all_meshes[0];

		
		current_level.clean_level();
		ai_bt.clean_tree();
	}
	
	void game_manager::poll_controls(double dt) {
		for (int i = 0; i < 2; i++) {
			if ((GetKeyState(button_mappings[i]) & 0x8000) != 0) {
				// prevent underflow
				if (buttons[i] + dt < 0) buttons[i] = 0;
				// button is pressed
				buttons[i] += dt;
				buttons_frame[i]++;
			} else {
				// reset button
				buttons[i] = 0;
				buttons_frame[i] = 0;
			}
		}
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