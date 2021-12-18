#include "../hpp/game_manager.hpp"


namespace crow {

	void game_manager::load_mesh_data(std::string filename, mesh_a& temp, int index) {
		// this method currently is only configured to take static meshes
		load_bin_data(filename.c_str(), temp);
		all_meshes[index].s_mesh = new mesh_s(clip_mesh(temp));
		p_impl->create_vertex_buffer(all_meshes[index].vertex_buffer, all_meshes[index].index_buffer, *all_meshes[index].s_mesh);
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

		// NOTES: important, because every mesh_info holds a pointer to an animator, that means that for animated meshes you will need an entry for every entity that will require it.
		// so if you have more ai and they all use the player mesh, you will need to load 1 player mesh for each additional ai as an example
		// making more AI units
		all_meshes[mesh_types::AI_1].a_mesh = new mesh_a();
		load_bin_data("res/meshes/guy.bin", *all_meshes[mesh_types::AI_1].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::AI_1].vertex_buffer, all_meshes[mesh_types::AI_1].index_buffer, *all_meshes[mesh_types::AI_1].a_mesh);

		all_meshes[mesh_types::AI_2].a_mesh = new mesh_a();
		load_bin_data("res/meshes/guy.bin", *all_meshes[mesh_types::AI_2].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::AI_2].vertex_buffer, all_meshes[mesh_types::AI_2].index_buffer, *all_meshes[mesh_types::AI_2].a_mesh);

		all_meshes[mesh_types::AI_3].a_mesh = new mesh_a();
		load_bin_data("res/meshes/guy.bin", *all_meshes[mesh_types::AI_3].a_mesh);
		p_impl->create_vertex_buffer(all_meshes[mesh_types::AI_3].vertex_buffer, all_meshes[mesh_types::AI_3].index_buffer, *all_meshes[mesh_types::AI_3].a_mesh);

		// loading non-animated meshes
		mesh_a temp;
		load_mesh_data("res/meshes/floor1.bin", temp, mesh_types::CUBE);
		load_mesh_data("res/meshes/door2.bin", temp, mesh_types::DOOR);
		load_mesh_data("res/meshes/exit_light.bin", temp, mesh_types::EXIT_LIGHT);
		load_mesh_data("res/meshes/console1.bin", temp, mesh_types::CONSOLE1);
		load_mesh_data("res/meshes/console2.bin", temp, mesh_types::CONSOLE2);
		load_mesh_data("res/meshes/bed1.bin", temp, mesh_types::BED1);
		load_mesh_data("res/meshes/chair1.bin", temp, mesh_types::CHAIR1);
		load_mesh_data("res/meshes/electric_box1.bin", temp, mesh_types::ELECTRIC_BOX1);
		load_mesh_data("res/meshes/server_box1.bin", temp, mesh_types::SERVER_BOX1);
		load_mesh_data("res/meshes/sofa1.bin", temp, mesh_types::SOFA1);
		load_mesh_data("res/meshes/sofa2.bin", temp, mesh_types::SOFA2);
		load_mesh_data("res/meshes/sofa3.bin", temp, mesh_types::SOFA3);
		load_mesh_data("res/meshes/barrel1.bin", temp, mesh_types::BARREL1);
		load_mesh_data("res/meshes/barrel2.bin", temp, mesh_types::BARREL2);
		load_mesh_data("res/meshes/barrel3.bin", temp, mesh_types::BARREL3);
		load_mesh_data("res/meshes/crate1.bin", temp, mesh_types::CRATE1);
		load_mesh_data("res/meshes/crate2.bin", temp, mesh_types::CRATE2);
		load_mesh_data("res/meshes/crate3.bin", temp, mesh_types::CRATE3);
		load_mesh_data("res/meshes/desk1.bin", temp, mesh_types::DESK1);
		load_mesh_data("res/meshes/desk2.bin", temp, mesh_types::DESK2);
		load_mesh_data("res/meshes/desk3.bin", temp, mesh_types::DESK3);
		load_mesh_data("res/meshes/light_box.bin", temp, mesh_types::LIGHT_BOX);
		load_mesh_data("res/meshes/console3.bin", temp, mesh_types::CONSOLE3);
		load_mesh_data("res/meshes/disk.bin", temp, mesh_types::DISK);
	}

	void game_manager::load_texture_data() {
		textures.resize(texture_list::COUNT);

		p_impl->create_texture("res/textures/player.dds", textures[texture_list::PLAYER]);
		p_impl->create_texture("res/textures/enemy1.dds", textures[texture_list::AI]);
		p_impl->create_texture("res/textures/npc.dds", textures[texture_list::NPC]);
		p_impl->create_texture("res/textures/floor_1.dds", textures[texture_list::FLOOR0]);
		p_impl->create_texture("res/textures/T_49_Diffuse.dds", textures[texture_list::FLOOR1]);
		
		p_impl->create_texture("res/textures/T_04_Diffuse.dds", textures[texture_list::FLOOR2]);
		p_impl->create_texture("res/textures/T_23_Diffuse.dds", textures[texture_list::FLOOR2_2]);
		p_impl->create_texture("res/textures/T_11_Diffuse2.dds", textures[texture_list::FLOOR4]);
		p_impl->create_texture("res/textures/T_05_Diffuse.dds", textures[texture_list::FLOOR5]);
		p_impl->create_texture("res/textures/T_07_Diffuse.dds", textures[texture_list::FLOOR5_2]);
		p_impl->create_texture("res/textures/T_14_Diffuse.dds", textures[texture_list::FLOOR5_ENTRANCE]);
		p_impl->create_texture("res/textures/T_02_Diffuse.dds", textures[texture_list::FLOOR5_SD]);
		p_impl->create_texture("res/textures/T_19_Diffuse.dds", textures[texture_list::FLOOR_CARGO]);

		p_impl->create_texture("res/textures/T_48_Diffuse.dds", textures[texture_list::WALL1]);
		p_impl->create_texture("res/textures/T_08_Diffuse.dds", textures[texture_list::WALL2]);
		p_impl->create_texture("res/textures/Texture_3_Diffuse.dds", textures[texture_list::WALL4]);

		p_impl->create_texture("res/textures/door_open.dds", textures[texture_list::DOOR_OPEN]);
		p_impl->create_texture("res/textures/door_closed.dds", textures[texture_list::DOOR_CLOSED]);
		p_impl->create_texture("res/textures/door_exit.dds", textures[texture_list::DOOR_EXIT]);
		p_impl->create_texture("res/textures/door_off.dds", textures[texture_list::DOOR_BROKEN]);
		p_impl->create_texture("res/textures/exit_light_d.dds", textures[texture_list::EXIT_LIGHT_D]);
		p_impl->create_texture("res/textures/exit_light_s.dds", textures[texture_list::EXIT_LIGHT_S]);
		p_impl->create_texture("res/textures/console1_d.dds", textures[texture_list::CONSOLE1_D]);
		p_impl->create_texture("res/textures/console1_d_purple.dds", textures[texture_list::CONSOLE1_D_PURPLE]);
		p_impl->create_texture("res/textures/console1_d_red.dds", textures[texture_list::CONSOLE1_D_RED]);
		p_impl->create_texture("res/textures/console1_s.dds", textures[texture_list::CONSOLE1_S]);
		p_impl->create_texture("res/textures/console2.dds", textures[texture_list::CONSOLE2]);

		p_impl->create_texture("res/textures/bed1.dds", textures[texture_list::BED1]);
		p_impl->create_texture("res/textures/chair1.dds", textures[texture_list::CHAIR1]);
		p_impl->create_texture("res/textures/electric_box1.dds", textures[texture_list::ELECTRIC_BOX1]);
		p_impl->create_texture("res/textures/server_box1.dds", textures[texture_list::SERVER_BOX1]);
		p_impl->create_texture("res/textures/sofa1.dds", textures[texture_list::SOFA1]);
		p_impl->create_texture("res/textures/sofa2.dds", textures[texture_list::SOFA2]);
		p_impl->create_texture("res/textures/sofa3.dds", textures[texture_list::SOFA3]);

		p_impl->create_texture("res/textures/crates_barrels.dds", textures[texture_list::CRATE_BARREL]);
		p_impl->create_texture("res/textures/crates_barrels_e.dds", textures[texture_list::CRATE_BARREL_E]);
		p_impl->create_texture("res/textures/desk1_2.dds", textures[texture_list::DESK12]);
		p_impl->create_texture("res/textures/desk3.dds", textures[texture_list::DESK3]);
		p_impl->create_texture("res/textures/light_box.dds", textures[texture_list::LIGHT_BOX]);

		p_impl->create_texture("res/textures/console3_d.dds", textures[texture_list::CONSOLE3]);
		p_impl->create_texture("res/textures/console3_d_red.dds", textures[texture_list::CONSOLE3_RED]);
		p_impl->create_texture("res/textures/console3_s.dds", textures[texture_list::CONSOLE3_E]);
		p_impl->create_texture("res/textures/shadow_full.dds", textures[texture_list::SHADOW]);

		p_impl->create_texture("res/textures/gui/select.dds", textures[texture_list::GUI_SELECT]);
		p_impl->create_texture("res/textures/gui/select2.dds", textures[texture_list::GUI_SELECT2]);
		p_impl->create_texture("res/textures/gui/pause.dds", textures[texture_list::GUI_PAUSE]);
		p_impl->create_texture("res/textures/gui/Crows_Nest.dds", textures[texture_list::GUI_LOGO]);

		p_impl->create_texture("res/textures/effect/whitePuff00.dds", textures[texture_list::EFFECT_STEAM]);
		p_impl->create_texture("res/textures/effect/dust.dds", textures[texture_list::EFFECT_DUST]);
		p_impl->create_texture("res/textures/effect/star_08.dds", textures[texture_list::EFFECT_HIT1]);

		p_impl->create_texture("res/textures/splash/full-sail-university.160x100.dds", textures[texture_list::SPLASH_FS]);
		p_impl->create_texture("res/textures/splash/GPGames_LogoOriginal.dds", textures[texture_list::SPLASH_GD]);
		p_impl->create_texture("res/textures/splash/Logic_Visions2.dds", textures[texture_list::SPLASH_LV]);
	}

	void crow::game_manager::load_animation_data()
	{
		animators.resize(animator_list::COUNT);

		size_t i = animator_list::PLAYER;
		animators[i].animations.resize(3);
		anim_clip pbindpose;
		load_anim_data("res/animations/guy.anim", pbindpose);
		load_anim_data("res/animations/guyf.anim", animators[i].animations[0]);
		load_anim_data("res/animations/jogging.anim", animators[i].animations[1]);
		load_anim_data("res/animations/dying_2.anim", animators[i].animations[2]);
		get_inverted_bind_pose(pbindpose.frames[0], animators[i]);

		i = animator_list::AI;
		animators[i].animations.resize(2);
		load_anim_data("res/animations/slasher_run.anim", animators[i].animations[0]);
		load_anim_data("res/animations/slasher_attack.anim", animators[i].animations[1]);
		get_inverted_bind_pose(animators[i].animations[0].frames[0], animators[i]);

		// this animation is broken
		i = animator_list::EXIT_LIGHT;
		animators[i].animations.resize(1);
		load_anim_data("res/animations/exit_light.anim", animators[i].animations[0]);
		get_inverted_bind_pose(animators[i].animations[0].frames[0], animators[i]);

		// NOTES: we will have to load an animator per additional ai unit that gets added (important)
		i = animator_list::AI_1;
		for (size_t j = 0; j < 3; ++j) {
			animators[i].animations.resize(2);
			load_anim_data("res/animations/jogging.anim", animators[i].animations[0]);
			load_anim_data("res/animations/dying_2.anim", animators[i].animations[1]);
			get_inverted_bind_pose(pbindpose.frames[0], animators[i]);
			++i;
		}

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

		load_game();

		// open the game
		current_state = game_state::S_SPLASH_FS;
#ifndef NDEBUG
		current_state = game_state::MAIN_MENU;
#endif
		init_credits();
		 
		load_texture_data();
	}

	void game_manager::update()
	{
		timer.Signal(); float dt = static_cast<float>(timer.Delta());
		time_elapsed += timer.Delta();
		p_impl->update(dt);

		// updates that run irregardless of the game state
		current_message.update(dt);
		//left_click_time += dt;
		//right_click_time += dt;

		// capture mouse position
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(p_impl->hwnd, &p);
		mouse_pos.x = (float)p.x;
		mouse_pos.y = (float)p.y;

		// debug mode toggle, allows debug camera
		if (GetKeyState(VK_F1) & 0x8000) {
			// prevents toggle while the key is held down
			if (!pressing_key) {
				pressing_key = true;
				debug_mode = ai_m.debug_mode = !debug_mode;
				if (debug_mode) {
					printf("\nDEBUG MODE ENABLED\n");
				}
				else {
					printf("\nDEBUG MODE DISABLED\n");
				}
			}
		}
		else {
			// resets press after letting go of the key
			pressing_key = false;
		}

		// game state update
		switch (current_state) {
		case game_state::PAUSED:
			update_particles(dt);
			break;
		case game_state::PLAYING: {
			if (current_level.interacting && current_message.progress_max &&
				current_message.progress_max == current_message.progress) {
				// stops broken ambient sound when a console is repaired
				crow::object_type interact_type = current_level.interacting->type;
				if (interact_type == crow::object_type::POWER_CONSOLE ||
					interact_type == crow::object_type::PRESSURE_CONSOLE ||
					interact_type == crow::object_type::OXYGEN_CONSOLE
					) crow::audio::stop_bgs();

				current_level.interacting->activate(*this);
				current_level.interacting = nullptr;
			}

			// the last thing that happens in update should always be player controls
			poll_controls(dt);

			// all ai updates (player and enemy) here
			if (current_level.found_ai) {
				if (true) ai_bt.run(dt);
				if (ai_bt.aim->killed_target) {
					int i = emitters.size();
					emitters.push_back(emitter_sp());
					emitters[i].room_id = room_with_enemy();
					emitters[i].life_span = 0.25f;

					if (ai_bt.aim->killed_target->type == crow::object_type::PLAYER) {
						emitters[i].type = emitter_type::PLAYER_BLOOD;
						emitters[i].prototype.pos = entities.get_world_position(0);
						emitters[i].prototype.pos.y += 0.5f;
					} else if (ai_bt.aim->killed_target->type != crow::object_type::AI){
						emitters[i].type = emitter_type::OBJ_DAMAGE;

						// figure out POS
						float2e tpos = tpos = current_level.selected_room->tiles.get_tile_wpos(ai_bt.aim->killed_target->x, ai_bt.aim->killed_target->y);

						emitters[i].prototype.pos = float3e(tpos.x, 0.5f, tpos.y);
						//emitters[i].prototype.pos = state.entities.get_world_position(object_indices.back());
					} else {
						emitters.pop_back();
					}
					ai_bt.aim->killed_target = nullptr;
				}

				// NOTE: This is only running the first AI, we should run the AI's based on which are in the current level instead
					size_t j = crow::entity::AI_1;
					for (size_t i = 0; i < current_level.units; ++i) {
						// if the AI is dead don't run the tree
						if (!live_entities_inter[ai_managers[i].inter_index]->is_active) {
							entities.mesh_ptrs[ai_managers[i].index]->animator->freeze_frame(1);
							entities.velocities[ai_managers[i].index] = { 0.f, 0.f, 0.f };
							++j;
							continue;
						}
						ai_bt2.aim = &ai_managers[i];
						ai_bt2.run(dt);

						// TODO: PLEASE CHANGE THIS, THERE IS NO REASON WHY A MATRIX SHOULD BE LOCATED IN THE BEHAVIOR TREE, IT JUST COMPLICATES A SIMPLE PROCESS
						ai_bt2.e_matrix.scale = { 0.25f, 0.25f, 0.25f };
						ai_bt2.e_matrix.rotate_y_axis_from_velocity(entities.velocities[j]);
						ai_bt2.e_matrix.update_position(entities.world_matrix[j]);
						ai_bt2.e_matrix.update();
						entities.world_matrix[j] = ai_bt2.e_matrix.final_matrix;
						++j;
					}
				//}
			}

			// making AI model face its velocity
			ai_bt.e_matrix.rotate_y_axis_from_velocity(entities.velocities[crow::entity::SPHYNX]);
			ai_bt.e_matrix.update_position(entities.world_matrix[crow::entity::SPHYNX]);
			ai_bt.e_matrix.update();
			entities.world_matrix[(int)crow::entity::SPHYNX] = ai_bt.e_matrix.final_matrix;
			//}

			//current_level.selected_room->tiles.debug_print_map();
			// animations need to be updated before checking if the player is alive
			update_animations(dt);
			update_particles(dt);
			entities.update_transform_data(dt);
			
			// update self-destruct timer
			if (self_destruct_timer > 0) {
				self_destruct_timer -= dt;

				// ran out of time, you dead boi
				if (self_destruct_timer <= 0) {
					// forcibly end the game
					end_game();
					prev_state = current_state = game_state::GAME_OVER;
					state_time = 0;
					audio::play_bgm(audio::BGM::GAME_OVER);
					break;
				}
			}

			// check for worker alive to end the game if he is dead
			if (!player_data.player_interact.is_active) {
				player_data.path_result.clear();
				game_over();
				break;
			}

			// input updates should be done if the player is alive
			if (player_data.player_interact.is_active) {
				l_click_update();
				r_click_update();
			}

			// movement update should be done if the player is still alive
			crow::path_through(player_data, entities, static_cast<size_t>(crow::entity::WORKER), dt);
			// check if we've beaten the game before attempting to continue
			if (current_state != game_state::PLAYING) break;

			current_level.selected_room->update_room_doors(textures, entities);

			// all this just to update the angle of the model of the player
			player_data.p_matrix.scale = { 0.25f, 0.25f, 0.25f };
			player_data.p_matrix.rotate_y_axis_from_velocity(entities.velocities[(int)crow::entity::WORKER]);
			player_data.p_matrix.update_position(entities.world_matrix[(int)crow::entity::WORKER]);
			player_data.p_matrix.update();
			entities.world_matrix[(int)crow::entity::WORKER] = player_data.p_matrix.final_matrix;

			// check for worker alive to end the game if he is dead
			if (!player_data.player_interact.is_active) {
				player_data.path_result.clear();
				game_over();
				break;
			}

			sound_updates(dt);
			room_updates(dt);
			audio::update_audio_timers(this, dt);

			// fetch any message that might have been summoned by the player object
			if (current_level.msg.time_remaining > 0) {
				// steal the info
				current_message = current_level.msg;
				// mark as read
				current_level.msg.time_remaining = 0;
				// read: 2:11 PM
			}

			if (c_buffered_message.wait > 0) {
				c_buffered_message.wait -= dt;

				// trigger the message
				if (c_buffered_message.wait <= 0) {
					// defer this message until the current message passes
					if (current_message.time_remaining > 0) {
						c_buffered_message.wait += 0.5;
					}
					else {
						current_message = c_buffered_message.b_message;

						// trigger the exit function (if applicable)
						if (c_buffered_message._exit_function) {
							(this->*c_buffered_message._exit_function)();
						}
					}
				}
			}
			
			break;
		case game_state::GAME_WIN: // falling case
			if (state_time > 5.f) {
				end_game();
				current_state = prev_state = game_state::CREDITS;
				state_time = 0;
				audio::play_bgm(audio::BGM::GAME_WIN);
				break;
			}
		case game_state::LEVEL_WIN:
			poll_controls(dt);
			if (current_level.found_ai) { ai_bt.run(dt); }

			// making AI model face its velocity
			ai_bt.e_matrix.rotate_y_axis_from_velocity(entities.velocities[(int)crow::entity::SPHYNX]);
			ai_bt.e_matrix.update_position(entities.world_matrix[(int)crow::entity::SPHYNX]);
			ai_bt.e_matrix.update();
			entities.world_matrix[(int)crow::entity::SPHYNX] = ai_bt.e_matrix.final_matrix;

			update_animations(dt);
			update_particles(dt);
			entities.update_transform_data(dt);

			if (current_state != game_state::GAME_WIN && state_time > 4.5f && buttons_frame[controls::l_mouse] == 1) {
				// unload the previous level
				unload_level();

				// load the next level
				load_level(level_number);

				// save game to file
				save_game();
			}
			break;
		case game_state::CREDITS:
			if (state_time > 81.f/1.9f) current_state = game_state::MAIN_MENU;
			break;
		case game_state::S_SPLASH_FS:
		case game_state::S_SPLASH_GD:
		case game_state::S_SPLASH_LV:
			// splash time has overstayed its welcome
			if (state_time > 4.0f) {
				// this increments the splash screen to its next splash screen
				current_state = (game_state)((int)current_state + 1);
				current_state = (game_state)((int)current_state % ((int)game_state::S_SPLASH_LV + 1));
				// lol casting.
			}
			break;
		}
		}

		// debug mode updates
		if (debug_mode) {
			float4e translate = { 0, 0, 0, 0 };
			p_impl->draw_path(ai_m.path, crow::float4e(1.f, 0.f, 0.f, 1.f));
			p_impl->draw_path(player_data.path_result, crow::float4e(0.f, 0.f, 1.f, 1.f));
			// debug camera controls
			/* WASD = basic movement                                                     */
			/* ARROW KEYS = rotate camera                                                */
			/* Z/X = barrel roll camera                                                  */
			/* LSHIFT/SPACE = move up/down                                               */
			/* RSHIFT (hold) = don't apply rotation while moving                         */
			/* R = reset camera to room's initial camera                                 */

			// movement for camera
			if ((GetKeyState('W') & 0x8000) != 0) translate.z += view.debug_camera_mspeed * dt;
			if ((GetKeyState('S') & 0x8000) != 0) translate.z -= view.debug_camera_mspeed * dt;
			if ((GetKeyState('A') & 0x8000) != 0) translate.x -= view.debug_camera_mspeed * dt;
			if ((GetKeyState('D') & 0x8000) != 0) translate.x += view.debug_camera_mspeed * dt;
			if ((GetKeyState(VK_LSHIFT) & 0x8000) != 0) translate.y -= view.debug_camera_mspeed * dt;
			if ((GetKeyState(VK_SPACE) & 0x8000) != 0) translate.y += view.debug_camera_mspeed * dt;

			// rotation for camera
			if ((GetKeyState(VK_UP) & 0x8000) != 0) view.rotation.x += view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_DOWN) & 0x8000) != 0) view.rotation.x -= view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_LEFT) & 0x8000) != 0) view.rotation.y += view.debug_camera_rspeed * dt;
			if ((GetKeyState(VK_RIGHT) & 0x8000) != 0) view.rotation.y -= view.debug_camera_rspeed * dt;
			if ((GetKeyState('X') & 0x8000) != 0) view.rotation.z += view.debug_camera_rspeed * dt;
			if ((GetKeyState('Z') & 0x8000) != 0) view.rotation.z -= view.debug_camera_rspeed * dt;

			// apply rotation to movement
			view.update_rotation_matrix();
			// but not if rshift is held
			if ((GetKeyState(VK_RSHIFT) & 0x8000) == 0) translate = MatrixVectorMult(translate, view.rot_mat);

			// update the position
			view.position.x += translate.x;
			view.position.y += translate.y;
			view.position.z += translate.z;


			if ((GetKeyState('R') & 0x8000) != 0 && current_level.selected_room) {
				crow::update_room_cam(pac(cam_pos), pac(cam_rotation), view);
				change_room_tex();
			}

			// VERY LAST thing to do should be to update the camera
			view.update();
		}

		state_time += dt;
		// band-aid for re-setting state_time and prev_state
		if (prev_state != current_state) {
			// play title theme on title screen
			if (current_state == game_state::MAIN_MENU) audio::play_bgm(audio::BGM::TITLE);
			prev_state = current_state;
			state_time = 0;
		}
	}

	void game_manager::update_animations(double dt) {
		// update all animations for all animated entities
		// there is only 2 animated entities so lets only loop 2 times instead of 100+ times
		for (uint32_t i = 0; i < 5 /*entities.current_size*/; i++) {
			if (entities.mesh_ptrs[i]->animator) {
				entities.mesh_ptrs[i]->animator->update(entities.framexbind[i], static_cast<float>(dt));
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

	void game_manager::update_particles(double dt) {
		/* // emitter test stuff
		if (emitters.size() != 8) {
			emitters.resize(8);

			emitters[0].room_id = 1;
			emitters[0].type = emitter_type::OBJ_DAMAGE_V2;
			emitters[0].prototype.pos = float3e(-5.5f, 0.5f, 5);
			emitters[0].prototype.texture = textures[texture_list::EFFECT_HIT1];


			emitters[1].type = emitter_type::PLAYER_BLOOD;
			emitters[1].prototype.pos = float3e(2, 0, 5);

			
			emitters[2].type = emitter_type::BROKEN_WIRE;
			emitters[2].prototype.pos = float3e(2, 0, -5);
			
			emitters[3].type = emitter_type::WATER_SPRAY;
			emitters[3].prototype.pos = float3e(-2, 0, -5);
			
			emitters[5].type = emitter_type::STEAM_V2;
			emitters[5].prototype.pos = float3e(-2, 0, 5);
			emitters[5].prototype.texture = textures[texture_list::EFFECT_STEAM];

			emitters[4].type = emitter_type::BROKEN_MACHINERY;
			emitters[4].prototype.pos = float3e(2, 0, 0);

			emitters[7].type = emitter_type::DUST;
			emitters[7].prototype.texture = textures[texture_list::EFFECT_DUST];
			emitters[7].prototype.pos = float3e(-4, 0, 0);

			emitters[6].type = emitter_type::GAS_SPRAY;
			emitters[6].prototype.pos = float3e(-4, 0, -5);
		}
		//*/
		

		for (int i = 0; i < emitters.size(); i++) {
			if (emitters[i].room_id != current_level.selected_room->id) continue;

			// makes sure broken emitter only plays if the console is not working
			if (emitters[i].type == emitter_type::BROKEN_MACHINERY && current_level.selected_room->has_broken_console() != 1) continue;

			p_impl->set_sorted_particles(emitters[i], emitters[i].type);
			p_impl->update_sorted_particles(emitters[i], dt);
		}
	}

	bool game_manager::l_click_update() {
		if (buttons_frame[controls::l_mouse] != 1) return false;

		if (current_message.progress_max > 0) current_message = crow::message();


		crow::room* selected_room = current_level.selected_room;
		if (!selected_room || !selected_room->has_player) {
			return true;
		}

		//if (minimap.inside_minimap(mouse_pos_gui)) {
		//	return true;
		//}

		player_data.interacting = false;
		player_data.target = nullptr;
		ImVec2 wh = get_window_size();

		float3e mouse_point = crow::mouse_to_floor(view, mouse_pos, static_cast<int>(wh.x), static_cast<int>(wh.y));
		if (mouse_point.y == -1) {
			return true;
		}
		const crow::tile* clicked_tile =
			selected_room->get_tile_at(float2e(mouse_point.x, mouse_point.z));

		if (!clicked_tile) {
			return true;
		}

		crow::interactible* target = nullptr;

		for (auto& i : selected_room->objects) {
			if (clicked_tile->row != i->y || clicked_tile->col != i->x) {
				continue;
			}

			// set target to i then break
			target = i;
			break;
		}
		// the player's position
		const float3e p_pos = entities.get_world_position(
			static_cast<size_t>(crow::entity::WORKER));

		// the player's tile
		const crow::tile* p_tile =
			selected_room->get_tile_at({ p_pos.x, p_pos.z });

		if (!p_tile) {
			return true;
		}

		if (p_tile == clicked_tile) {
			player_data.path_result.clear();
			return true;
		}

		// logic for clicking on an interactible
		if (target) {
			// preparing to interact
			player_data.interacting = true;
			player_data.target = target;

			// finding the adjacent tile to the interactible
			float2e adjacent_tile =
				float2e{ static_cast<float>(p_tile->col) -
								static_cast<float>(clicked_tile->col),
							static_cast<float>(p_tile->row) -
								static_cast<float>(clicked_tile->row) };

			adjacent_tile = adjacent_tile.normalize();
			for (int i = 0; i < 2; i++) {
				if (adjacent_tile[i] >= 0.5f) {
					adjacent_tile[i] = 1.f;
					continue;
				}
				else if (adjacent_tile[i] <= -0.5f) {
					adjacent_tile[i] = -1.f;
				}
			}

			adjacent_tile = {
				adjacent_tile.x + static_cast<float>(clicked_tile->col),
				adjacent_tile.y + static_cast<float>(clicked_tile->row) };

			adjacent_tile =
				selected_room->get_tile_wpos(static_cast<int>(adjacent_tile.x),
					static_cast<int>(adjacent_tile.y));

			// path finding to a tile next to the interactible
			std::vector<float2e> temporary_results =
				selected_room->get_path(float2e(p_pos.x, p_pos.z), adjacent_tile);

			// set the worker's path
			player_data.path_result = temporary_results;
		}
		else { // normal tile movement
			std::vector<float2e> temporary_results =
				current_level.selected_room->get_path(
					float2e(p_pos.x, p_pos.z),
					float2e(mouse_point.x, mouse_point.z));

			if (temporary_results.empty()) {
				return true;
			}

			// set the worker's path
			player_data.path_result = temporary_results;
		}

		if (!player_data.path_result.empty()) {
			crow::audio::add_footstep_sound(
				(float4x4_a*)&entities.world_matrix[static_cast<size_t>(
					crow::entity::WORKER)], 0.5f);
		}

		return true;
	}

	bool game_manager::r_click_update() {
		if (minimap.inside_minimap(mouse_pos_gui)) return false;

		if (buttons_frame[controls::r_mouse] != 1) {
			return false;
		}
		// stopping the player and resetting interaction handles
		player_data.path_result.clear();
		if (current_message.progress_max > 0) current_message = crow::message();
		player_data.interacting = false;
		player_data.target = nullptr;
		return true;
	}

	bool game_manager::enemy_and_player_in_same_room() {
		for (auto& rv : current_level.rooms) {
			for (auto& r : rv) {
				if (r.id <= 0) { // not valid room
					continue;
				}
				if (r.has_ai && r.has_player) return true;
			}
		}
		return false;
	}

	int game_manager::room_with_worker() {
		for (auto& rv : current_level.rooms) {
			for (auto& r : rv) {
				if (r.id <= 0) { // not valid room
					continue;
				}
				if (r.has_player) return r.id;
			}
		}
		return 0;
	}

	int game_manager::room_with_enemy() {
		for (auto& rv : current_level.rooms) {
			for (auto& r : rv) {
				if (r.id <= 0) { // not valid room
					continue;
				}
				if (r.has_ai) return r.id;
			}
		}
		return 0;
	}

	void game_manager::room_updates(double dt) {
		// updating the heat values for the doors in the level and oxygen
		for (auto& rv : current_level.rooms) {
			for (auto& r : rv) {
				if (r.id <= 0) { // not valid room
					continue;
				}

				// updating oxygen less optimal as checked every loop but saves me the trouble of having to loop twice to keep doors updated
				if (current_level.oxygen_console && r.has_player && current_level.oxygen_console->is_broken) {
					// tutorial thing for pressure console broken
					//if (!first_pressure_console_break) {
					//	c_buffered_message.set(message("The pressure console has been broken.", 4.0f), 0.02f, t_pressure_message2);
					//
					//	first_pressure_console_break = true;
					//}
					// temporarily commenting this out to prevent unpredictable behavior in the final version
					// TODO::complete and test this

					// decreases oxygen level
					r.oxygen -= static_cast<float>(dt);

					// this kills the worker
					if (r.oxygen <= 0) {
						r.oxygen = 0;
						player_data.player_interact.dissable();
						return;
					}
				}

				for (auto& o : r.objects) {
					if (o->type != crow::object_type::DOOR) {
						continue;
					}
					/*std::string hval = "\nheat: " + std::to_string(o->heat);*/
					// for doors update the heat value
					if (o->heat > 0.05f) {
						o->heat -= static_cast<float>(dt) * 0.25f;
					}
					else if (o->heat < 0.05f) {
						o->heat += static_cast<float>(dt) * 0.25f;
					}
					else {
						o->heat = 0.f;
					}
				}
			}
		}

		// pressure console updates
		if (current_level.pressure_console && current_level.pressure_console->is_broken) {
			// pressure is decreasing!
			current_level.pressure_console->pressure -= static_cast<float>(dt);

			// this kills the worker
			if (current_level.pressure_console->pressure <= 0.f) {
				current_level.pressure_console->pressure = 0.f;
				//game_over();
				player_data.player_interact.dissable();
				return;
			}
		}

		// updating events
		for (int i = 0; i < current_level.selected_room->event_triggers.size(); i++) {
			// don't check for events if the room doesn't have the player in it
			if (!current_level.selected_room->has_player) break;

			event_trigger& t = current_level.selected_room->event_triggers[i];

			// check if event is triggered
			if (t.within_bounds(entities.get_world_position(crow::entity::WORKER))) {
				// execute event
				(this->*t._event)();
				// pop event from stack
				current_level.selected_room->event_triggers.erase(current_level.selected_room->event_triggers.begin() + i);
			}
		}
	}

	void game_manager::sound_updates(double dt) {
		// this will make the enemy play foostep sounds, but only if they're moving
		if (!crow::audio::enemy_isnt_moving(this)) {
			crow::audio::add_footstep_sound_e(
				(float4x4_a*)&entities.world_matrix[static_cast<size_t>(
					crow::entity::WORKER)], 0.3f);
		}

		// processing for enemy appear sound
		if (current_level.selected_room->has_player && current_level.selected_room->has_ai) {
			if (enemy_appear_sound_cooldown <= 0) {
				if (self_destruct_timer < 0) audio::play_bgm(audio::BGM::DETECTED);
				crow::audio::play_sfx(crow::audio::SFX::ENEMY_APPEAR);

				// display the enemy appearance message for the first time it appears
				if (!enemy_first_appearance) {
					// if you're not currently interacting with an object, display the message immediately.
					// otherwise, defer it just enough to finish interacting
					if (!(current_message.progress_max > current_message.progress))
						current_message = message("If that thing gets you, you're dead! RUN!", 3.5f);
					else c_buffered_message.set(message("If that thing gets you, you're dead! RUN!", 3.5f),
						current_message.progress_max - current_message.progress, nullptr);

					// don't display this again
					enemy_first_appearance = true;
				}
			}
			enemy_appear_sound_cooldown = enemy_appear_sound_max_cooldown;
		} else if (self_destruct_timer < 0) {
			int reset_bgm = enemy_appear_sound_cooldown > 0 ? 1 : 0;

			// decrement the sound cooldown to allow it to play again
			if (!enemy_and_player_in_same_room()) enemy_appear_sound_cooldown -= dt;
			if (enemy_appear_sound_cooldown <= 0) {
				enemy_appear_sound_cooldown = 0;
				reset_bgm = reset_bgm + 1;
			}

			// sets the bgm back to normal when the encounter period wears off
			if (reset_bgm == 2) {
				audio::play_bgm(audio::BGM::NORMAL);
			}
		}

		// setup for checking console sound
		int console_status = current_level.selected_room->has_broken_console();
		bool bgs_playing = crow::audio::bgs_handle != -1;

		// no console in room, make sure bgs isnt playing
		if (console_status == 0) {
			// stop sound if playing
			if (bgs_playing) crow::audio::stop_bgs();
			// broken console in room, play broken electronics sound
		}
		else if (console_status == 1) {
			if (!bgs_playing) crow::audio::play_bgs(crow::audio::SFX::CONSOLE_BROKEN);
		}
		else if (console_status == -1) {
			if (!bgs_playing) crow::audio::play_bgs(crow::audio::SFX::CONSOLE_WORKING, false);
		}
	}

	void game_manager::render()
	{
		p_impl->set_render_target_view();

		switch (current_state) {
		case game_state::SETTINGS:
			// case falls if in-game, draw nothing otherwise
			if (!current_level.rooms.size()) break;
		case game_state::GAME_WIN: // falling case
		case game_state::LEVEL_WIN: // falling case
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
		//p_impl->draw_mesh(view);

		if (current_level.selected_room && entities.current_size > 0) {
			if (self_destruct_timer < 0) {
				// normal lighting based on power
				if (current_level.power_console != nullptr && current_level.power_console->is_broken) {
					entities.amblight = { 0.25f, 0.25f, 0.25f, 0 };
				}
				else {
					entities.amblight = { 0.5f, 0.5f, 0.5f, 0 };
				}
			} else {
				// red flash for sd timer

				// truncated timer
				int int_time = (int)self_destruct_timer;
				float ms_remaining = self_destruct_timer - int_time;

				// whether or not the listed time is odd
				int odd = int_time % 2;

				if (odd) {
					entities.amblight = { 0.50f + ms_remaining / 4.0f, 0.5f, 0.5f, 0 };
				} else {
					entities.amblight = { 0.75f - ms_remaining / 4.0f, 0.5f, 0.5f, 0 };
				}
			}

			// drawing all entities
			p_impl->draw_entities(entities, current_level.selected_room->object_indices, view);

			// vector storing the index of the shadow blob entity used for the player and AI
			std::vector<size_t> svec;
			svec.push_back(crow::entity::SHADOW);

			// drawing shadow at player's position
			if (current_level.selected_room->has_player) {
				// we must copy the player's matrix
				entities.world_matrix[crow::entity::SHADOW] = entities.world_matrix[crow::entity::WORKER];

				// if the player is alive
				if (player_data.player_interact.is_active) {
					// scale and position the shadow
					entities.scale_world_matrix(crow::entity::SHADOW, 1.6f, 4.f, 1.2f);
					entities.set_world_position(crow::entity::SHADOW, 0.f, 0.055f, 0.f, false);

					// draw call for only the shadow
					p_impl->draw_entities(entities, svec, view);
				}
				else {
					// updating shadow spawn time
					shadow_spawn_timer += static_cast<float>(timer.Delta());
					crow::float3e foward;
					DirectX::XMFLOAT4X4 w;
					DirectX::XMStoreFloat4x4(&w, entities.world_matrix[crow::entity::SHADOW]);

					// retrieving the forward of our matrix
					foward.x = w.m[2][0];
					foward.y = w.m[2][1];
					foward.z = w.m[2][2];
					foward = foward.normalize(foward);

					// scaling and moving the shadow to match the body
					entities.scale_world_matrix(crow::entity::SHADOW, 1.2f, 4.f, 3.f);
					float xpos = w.m[3][0] + foward.x * 2;
					float zpos = w.m[3][2] + foward.z * 2;
					entities.set_world_position(crow::entity::SHADOW, xpos, 0.055f, zpos, false);

					// drawing the shadow after a second has passed
					if (shadow_spawn_timer >= 1.0f) {
						p_impl->draw_entities(entities, svec, view);
					}
				}
			}

			// drawing shadow at AI's position
			if (current_level.selected_room->has_ai) {
				entities.world_matrix[crow::entity::SHADOW] = entities.world_matrix[crow::entity::SPHYNX];
				// scaling and moving shadow to match the AI
				entities.scale_world_matrix(crow::entity::SHADOW, 0.5f, 1.f, 0.3f);
				entities.set_world_position(crow::entity::SHADOW, 0.f, 0.055f, 0.f, false);
				// drawing just the shadow again
				p_impl->draw_entities(entities, svec, view);
			}

			// drawing shadow at npc's position
			if (!current_level.selected_room->live_entities.empty()) {
				for (auto& e_indx : current_level.selected_room->live_entities) { // looping through all live entities in the room
					for (auto& m : ai_managers) { // looping through all ai units
						if (e_indx == m.inter_index) { // checking to see which unit is the one in the room
							if (live_entities_inter[e_indx]->is_active) { // if active, draw shadow
								entities.world_matrix[crow::entity::SHADOW] = entities.world_matrix[m.index];
								entities.scale_world_matrix(crow::entity::SHADOW, 1.6f, 4.f, 1.2f);
								entities.set_world_position(crow::entity::SHADOW, 0.f, 0.055f, 0.f, false);
								p_impl->draw_entities(entities, svec, view);
								return;
							}
						}
					}
				}
			}
		}
		p_impl->draw_debug_lines(view);
	}

	void game_manager::game_over() {
		const size_t index = static_cast<size_t>(entity::WORKER);
		/*player_data.path_result.clear();*/
		entities.velocities[index] = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (entities.mesh_ptrs[index]->animator &&
			!entities.mesh_ptrs[index]->animator->is_acting &&
			!entities.mesh_ptrs[index]->animator->performed_action) {

			entities.mesh_ptrs[index]->animator->switch_animation(animator::anim_type::DYING);
			entities.mesh_ptrs[index]->animator->performed_action = true;
		}
		else if (entities.mesh_ptrs[index]->animator &&
			entities.mesh_ptrs[index]->animator->performed_action &&
			!entities.mesh_ptrs[index]->animator->is_acting) {
			end_game();
			prev_state = current_state = game_state::GAME_OVER;
			state_time = 0;
			audio::play_bgm(audio::BGM::GAME_OVER);
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

		animators.clear();
		all_meshes.clear();
	}

	game_manager::game_manager()
	{

	}

	game_manager::~game_manager()
	{
		cleanup();
		audio::cleanup();

		// clearing texture data
		for (auto& m : textures) {
			if (m) {
				m->Release();
			}
		}
		textures.clear();
		
		// prevents memory leaks (????????)
		p_impl->swapchain->SetFullscreenState(false, nullptr);
		// delet this
		delete p_impl;
	}

	void game_manager::save_game() {
		try {
			std::fstream savefile;
			savefile.open("save.dat", std::ios::out);
			if (!savefile) {
				printf("Couldn't write to save file!\n");
			}
			else {
				std::string save_data;
				save_data += level_number;
				save_data.resize(14);

				memcpy(&save_data[1], &audio::all_volume, 4);
				memcpy(&save_data[5], &audio::bgm_volume, 4);
				memcpy(&save_data[9], &audio::sfx_volume, 4);

				BOOL fs;
				p_impl->swapchain->GetFullscreenState(&fs, nullptr);
				save_data[13] = fs;

				savefile.write(save_data.c_str(), save_data.length());
				savefile.close();
				printf("Sucessfully saved game.\n");
			}
		}
		catch (std::exception e) {
			printf(e.what());
		}

	}

	void game_manager::load_game() {
		try {
			std::fstream savefile;
			savefile.open("save.dat", std::ios::in);
			if (!savefile) {
				printf("Notice: No save file was loaded.\n");
				p_impl->swapchain->SetFullscreenState(true, nullptr);
			}
			else {
				char save_data[14];

				savefile.read(save_data, 14);

				level_number = save_data[0];

				memcpy(&audio::all_volume, &save_data[1], 4);
				memcpy(&audio::bgm_volume, &save_data[5], 4);
				memcpy(&audio::sfx_volume, &save_data[9], 4);

				bool fs = save_data[13];
				if (fs) p_impl->swapchain->SetFullscreenState(true, nullptr);
			}
		}
		catch (std::exception e) {
			printf(e.what());
		}
	}

	void game_manager::new_game() {
		load_all_meshes();
		load_animation_data();

		// prevents the game from crashing when you died while en-route to an interactable
		player_data.target = nullptr;
		player_data.interacting = false;

		//assigning animatiors, could be done inside function if meshes have been initialized
		all_meshes[mesh_types::PLAYER].animator = &animators[animator_list::PLAYER];
		all_meshes[mesh_types::AI_1].animator = &animators[animator_list::AI_1];
		all_meshes[mesh_types::AI_2].animator = &animators[animator_list::AI_2];
		all_meshes[mesh_types::AI_3].animator = &animators[animator_list::AI_3];
		all_meshes[mesh_types::AI].animator = &animators[animator_list::AI];
		all_meshes[mesh_types::EXIT_LIGHT].animator = &animators[animator_list::EXIT_LIGHT];

		// initializing main entities
		entities.allocate_and_init(crow::entity::COUNT);

		// creating player entity
		entities.mesh_ptrs[crow::entity::WORKER] = &all_meshes[mesh_types::PLAYER];
		entities.s_resource_view[crow::entity::WORKER] = textures[texture_list::PLAYER];

		// creating AI entity
		entities.mesh_ptrs[crow::entity::SPHYNX] = &all_meshes[mesh_types::AI];
		entities.s_resource_view[crow::entity::SPHYNX] = textures[texture_list::AI];

		// NOTES: for every additional ai unit you will have to make a new entity much like the worker and enemy ai
		// other AI entities
		entities.mesh_ptrs[crow::entity::AI_1] = &all_meshes[mesh_types::AI_1];
		entities.s_resource_view[crow::entity::AI_1] = textures[texture_list::NPC];

		entities.mesh_ptrs[crow::entity::AI_2] = &all_meshes[mesh_types::AI_2];
		entities.s_resource_view[crow::entity::AI_2] = textures[texture_list::NPC];

		entities.mesh_ptrs[crow::entity::AI_3] = &all_meshes[mesh_types::AI_3];
		entities.s_resource_view[crow::entity::AI_3] = textures[texture_list::NPC];

		// creating shadow entity
		entities.mesh_ptrs[crow::entity::SHADOW] = &all_meshes[mesh_types::DISK];
		entities.world_matrix[crow::entity::SHADOW];
		//entities.scale_world_matrix(crow::entity::SHADOW, 0.3f, 0.1f, 0.3f);
		entities.s_resource_view[crow::entity::SHADOW] = textures[texture_list::SHADOW];

		// creating floor entity
		entities.mesh_ptrs[entity::FLOOR] = &all_meshes[mesh_types::CUBE];
		// default floor texture, must be replaced every time the room has a different texture for floor
		entities.s_resource_view[entity::FLOOR] = textures[texture_list::FLOOR1];
		float4x4_a floor_size = (float4x4_a&)entities.world_matrix[entity::FLOOR];
		floor_size[0][0] = current_level.room_width;
		floor_size[1][1] = 0.1f;
		floor_size[2][2] = current_level.room_length;
		entities.world_matrix[entity::FLOOR] = (DirectX::XMMATRIX&)floor_size;

		// creating wall entities
		//up wall
		for (size_t i = entity::WALL_U; i <= entity::WALL_R; ++i) {
			float4x4_a wall_size = (float4x4_a&)entities.world_matrix[i];
			wall_size[1][1] = static_cast<float>(current_level.room_height);
			entities.mesh_ptrs[i] = &all_meshes[game_manager::mesh_types::CUBE];
			entities.s_resource_view[i] = textures[game_manager::texture_list::FLOOR1];

			float cwidth = static_cast<float>(current_level.room_width);
			float clenght = static_cast<float>(current_level.room_length);

			switch (i) {
			case entity::WALL_U: {
				wall_size[0][0] = cwidth;
				wall_size[2][2] = 0.1f;
				wall_size[3][2] = clenght / 2;
				break;
			}
			case entity::WALL_D: {
				wall_size[0][0] = cwidth;
				wall_size[2][2] = 0.1f;
				wall_size[3][2] = -clenght / 2.f;
				break;
			}
			case entity::WALL_L: {
				wall_size[0][0] = 0.1f;
				wall_size[2][2] = clenght;
				wall_size[3][0] = -cwidth / 2;
				break;
			}
			case entity::WALL_R: {
				wall_size[0][0] = 0.1f;
				wall_size[2][2] = clenght;
				wall_size[3][0] = cwidth / 2;
				break;
			}
			}

			entities.world_matrix[i] = (DirectX::XMMATRIX&)wall_size;
		}

		load_level(level_number);
		audio::play_bgm(audio::BGM::NORMAL);
		c_buffered_message.reset();
	}

	void game_manager::end_game() {
		audio::stop_bgm();
		audio::stop_bgs();
		// make sure none of these exist
		audio::clear_audio_timers();
		current_level.clean_level();
		c_buffered_message.reset();
		cleanup();
		entities.pop_all();
		ai_bt.clean_tree();
		ai_bt2.clean_tree();
	}

	void game_manager::change_level(int lv) {
		audio::stop_bgs();
		c_buffered_message.reset();

		// remove the worker from the level
		// checks every room instead of just the current room since you don't have
		// to be looking at the worker to clear the level
		for (auto& i : current_level.rooms) {
			for (auto& j : i) {
				for (int k = 0; k < j.object_indices.size(); k++) {
					if (j.object_indices[k] == 0) {
						j.object_indices.erase(j.object_indices.begin() + k);
						break;
					}
				}
				j.has_player = false;
			}
		}

		// after all clean up is done, it's time to start loading the next level
		if (lv - 1 == crow::final_level) {
			// we beat the game, so send us to the endgame sequence, whatever that may be
			current_state = prev_state = game_state::GAME_WIN;
			state_time = 0;
		} else {
			// continue with next level
			current_state = prev_state = game_state::LEVEL_WIN;
			state_time = 0;
			level_number = lv;
		}
	}

	void game_manager::load_level(int lv) {
		// setting this bool properly for the tutorial popup for when the enemy appears for the first time
		if (lv < 2) enemy_first_appearance = false; // allow popup to occur once
		else enemy_first_appearance = true; // don't allow popup to occur

		if (lv <= 3) first_pressure_console_break = false;
		else first_pressure_console_break = true;

		live_entities_inter.clear();

		// reset any variables that may need resetting
		player_data.player_interact.is_active = true;

		// NOTE: The main AI targets based on an interactible inside this vector, it uses the index which is stored in the interactible itself to know which one belongs to who
		live_entities_inter.push_back(&player_data.player_interact);

		// NOTE: you will need to set up these interactibles as well, to hold proper data
		ai1_it.entity_index = crow::entity::AI_1;
		ai2_it.entity_index = crow::entity::AI_2;
		ai2_it.entity_index = crow::entity::AI_3;

		// NOTE: These are the interactibles for the first 3 ai units I am making, for any other units you add, you will need to add an interactible just like this
		live_entities_inter.push_back(&ai1_it);
		live_entities_inter.push_back(&ai2_it);
		live_entities_inter.push_back(&ai3_it);

		// clear emitters
		emitters.resize(0);

		// so that the music doesnt restart for no reason
		enemy_appear_sound_cooldown = 0;

		// don't explode
		self_destruct_timer = -1;
		good_ending = false;

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
		//crow::update_room_cam(current_level.selected_room, view);
		crow::update_room_cam(pac(cam_pos), pac(cam_rotation), view);
		change_room_tex();
		current_level.p_inter = &player_data.player_interact;

		// setting up minimap
		minimap = crow::minimap({ 0.0f, 0.65f }, { 0.35f, 0.35f });
		minimap.map_minc = { -300, -300 };
		minimap.map_maxc = { 300, 300 };
		minimap.screen_minr = { 0.0f, 0.65f };
		minimap.screen_maxr = { 0.35f, 0.35f };
		minimap.resolution = { 1920, 1080 };
		minimap.set_window_size(get_window_size());
		minimap.current_level = &current_level;
		minimap.calculate_extents();

		// initialize ai
		ai_m.init_manager(&entities, &current_level, &live_entities_inter);
		ai_bt.aim = &ai_m;
		// NOTE: 0 is for normal ai 
		ai_bt.build_tree(0);

		// NOTES: you will need to initialize a manager for each ai unit and at least 1 tree with the desired behavior set
		// if you plan to recycle the same tree you will need to swap the manager 
		// from the tree per unit every frame and run the tree every time. Or you can just build additional trees but that will cost you
		ai_managers.resize(3);
		size_t ai_index = crow::entity::AI_1;
		int inter_index = 1;
		for (auto& m : ai_managers) {
			m.init_manager(&entities, &current_level, nullptr);
			m.index = ai_index++;
			// NOTE: IT MAY BE WISE TO USE ENUMS HERE INSTEAD, SO BE CAREFUL
			m.inter_index = inter_index++;
			// NOTE: If you don't set this value the AI this AI will break doors
			m.friendly = true;
			m.roam_speed = 2.f;
			m.roam_total = 20.f;
		}
		// NOTE: 1 is for passive npc ai
		ai_bt2.build_tree(1);
		// NOTE: we do not set the manager of the behavior tree here because we will be sharing it among all 3 units in this vector

		// NOTE: You could just make a different interactible alltogether but the one I am using here is just the base class, and because I am using base class
		// I need to set its type to AI so that the enemy ai can also see this unit in the map
		ai1_it.type = ai2_it.type = ai3_it.type = crow::object_type::AI;
		ai1_it.is_active = ai2_it.is_active = ai3_it.is_active = true;
		

		shadow_spawn_timer = 0.f;
		current_state = crow::game_manager::game_state::PLAYING;
	}

	void game_manager::unload_level() {
		// remove all entities except for the sphynx and worker
		while (entities.current_size > entity::COUNT) {
			entities.pop_back();
		}

		// store these temporarily to re-store them later
		DirectX::XMMATRIX* temp0 = entities.framexbind[0];
		DirectX::XMMATRIX* temp1 = entities.framexbind[1];

		// reset the sphynx and worker entity data
		entities.init_entity(0);
		entities.init_entity(1);
		entities.mesh_ptrs[0] = &all_meshes[mesh_types::PLAYER];
		entities.s_resource_view[0] = textures[texture_list::PLAYER];
		entities.mesh_ptrs[1] = &all_meshes[mesh_types::AI];
		entities.s_resource_view[1] = textures[texture_list::AI];

		// clear emitters
		emitters.resize(0);

		// re-store the framexbind. this is necessary to stop crashes.
		// we may want to change this approach in the future (reset instead of restore)
		entities.framexbind[0] = temp0;
		entities.framexbind[1] = temp1;

		// make sure none of these exist
		audio::clear_audio_timers();

		current_level.clean_level();
		ai_bt.clean_tree();
		ai_bt2.clean_tree();

		live_entities_inter.clear();
	}

	void game_manager::poll_controls(double dt) {
		for (int i = 0; i < 2; i++) {
			if ((GetKeyState(button_mappings[i]) & 0x8000) != 0) {
				// prevent underflow
				if (buttons[i] + dt < 0) buttons[i] = 0;
				// button is pressed
				buttons[i] += dt;
				buttons_frame[i]++;
			}
			else {
				// reset button
				buttons[i] = 0;
				buttons_frame[i] = 0;
			}
		}
	}

	void game_manager::imgui_centertext(std::string text, float scale, ImVec2 wh) {
		float f = ImGui::GetFontSize() / 13;
		ImGui::SetWindowFontScale(ImGui::GetWindowSize().x / 960.f * scale);
		float text_size =ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX((wh.x - text_size) / 2.0f);
		ImGui::Text(text.c_str());
		ImGui::SetWindowFontScale(f);
		f = ImGui::GetFontSize();
	}

	ImVec2 game_manager::get_window_size() {
		RECT r;
		GetClientRect(p_impl->hwnd, &r);

		return ImVec2((float)(r.right - r.left), (float)(r.bottom - r.top));
	}

	void game_manager::change_room_tex() {
		entities.s_resource_view[entity::FLOOR] = current_level.selected_room->floor_tex;
		entities.s_resource_view[entity::WALL_U] = current_level.selected_room->wall_tex_u;
		entities.s_resource_view[entity::WALL_D] = current_level.selected_room->wall_tex_d;
		entities.s_resource_view[entity::WALL_L] = current_level.selected_room->wall_tex_l;
		entities.s_resource_view[entity::WALL_R] = current_level.selected_room->wall_tex_r;
	}
}