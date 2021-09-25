#include "../hpp/game_state.hpp"

#include <liblava-extras/fbx.hpp>

#include "../hpp/audio.hpp"
#include "../hpp/camera.hpp"
#include "../hpp/cross.hpp"
#include "../hpp/interactible.hpp" 1

namespace crow {

void draw_menus(game_state& state, ImVec2 wh) {
  // the game is paused, so draw a pause menu
  if (state.current_state == state.PAUSED) {
    // set size parameters for the pause window
    ImVec2 pause_window_xy = {wh.x * 0.35f, wh.y * 0.275f};
    ImVec2 pause_window_wh = {wh.x * 0.3f, wh.y * 0.45f};
    ImGui::SetNextWindowPos(pause_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(pause_window_wh, ImGuiCond_Always);
    ImGui::Begin("Pause Window", 0,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize);

    // some text at the top of the window to let you know that the game is
    // paused right now this is some text, we may want to change this to an
    // image or somesuch later
    std::string ptext = "PAUSED";
    ImGui::SetWindowFontScale(wh.x / 960.f * 2.0f);
    float text_size = ImGui::GetFontSize() * ptext.size() / 2;
    ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
                    (text_size / 2.0f));
    ImGui::Text(ptext.c_str());

    ImVec2 pause_menu_button_wh = {pause_window_wh.x * 0.55f, wh.y * 0.05f};

    // TODO: the text buttons are placeholders, we will need proper image
    // buttons later draw all the menu options here
    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.19f});
    if (ImGui::Button("Resume", pause_menu_button_wh)) {
      state.current_state = state.PLAYING;
      crow::audio::play_sfx(crow::audio::MENU_OK);
    }

    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.27f});
    if (ImGui::Button("Main Menu", pause_menu_button_wh)) {
      state.current_state = state.MAIN_MENU;
      crow::audio::play_sfx(crow::audio::MENU_OK);
    }

    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.35f});
    if (ImGui::Button("Quit", pause_menu_button_wh)) {
      // no, that button doesn't quit the menu, it quits the game, and it
      // doesn't even go "are you sure?", it's just like x_x
      state.app->shut_down();
      crow::audio::play_sfx(crow::audio::MENU_OK);
    }

    ImGui::End();
  }

  // the game is MAIN MENU7, so draw the main menu
  if (state.current_state == state.MAIN_MENU) {
    // first things first, let's set the size of the main menu. it covers the
    // whole screen tho, so...
    ImVec2 mm_window_xy = {0, 0};
    ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
    // black background so you can't tell that there is anything going on behind
    // it, borderless so you can't tell it's just an imgui window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::SetNextWindowBgAlpha(1);
    // main menu drawing starts here
    ImGui::Begin("Main Menu", 0,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize);

    // as a placeholder, i have the game name as some tedext at the top, there
    // should probably be a logo or something here later on (TODO)
    std::string ptext = "Crow's Nest";
    ImGui::SetWindowFontScale(wh.x / 960.f * 2.0f);
    float text_size = ImGui::GetFontSize() * ptext.size() / 2;
    ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
                    (text_size / 2.0f));
    ImGui::SetCursorPosY(wh.y * 0.16f);
    ImGui::Text(ptext.c_str());

    // size of all menu options
    ImVec2 mm_button_wh = {wh.x * 0.55f, wh.y * 0.05f};

    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.65f});
    if (ImGui::Button("New Game", mm_button_wh)) {
      end_game(state);
      new_game(state);
      crow::audio::play_sfx(crow::audio::MENU_OK);
    }

    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.75f});
    if (ImGui::Button("Options", mm_button_wh)) {
      /* currently does nothing. TODO: this */
      crow::audio::play_sfx(crow::audio::MENU_OK);
    }

    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.85f});
    if (ImGui::Button("Quit", mm_button_wh)) {
      crow::audio::play_sfx(crow::audio::MENU_OK);
      state.app->shut_down();
    }
    ImGui::End();

    // no longer drawing borderless
    ImGui::PopStyleVar(1);
  }
}

void new_game(crow::game_state& state) {
  //-----map  generation testing----
  state.world_map = crow::world_map<5, 5>();
  // minimap logic
  state.minimap = crow::minimap({0.0f, 0.65f}, {0.4f, 0.35f});
  state.minimap.map_minc = {-300, -300};
  state.minimap.map_maxc = {300, 300};
  state.minimap.screen_minr = {0.0f, 0.65f};
  state.minimap.screen_maxr = {0.4f, 0.35f};
  state.minimap.resolution = {1920, 1080};
  state.minimap.set_window_size(state.app->window.get_size());
  state.world_map.generate_blocks(4);
  state.world_map.generate_rooms(6, 3);
  state.world_map.generate_adjacencies();
  state.minimap.populate_map_data(&state.world_map, state.app);

  // TEMPORARY
  // TODO: ADD ITEMS TO SHOWCASE ROOM
  item temp_item_a;
  item temp_item_b;
  sd_console* temp_sdc = new sd_console;
  temp_item_a.type = item_type::FLASK;
  temp_item_b.type = item_type::ALARM;
  temp_item_a.player_inv = temp_item_b.player_inv =
      &state.player_data.player_inventory;

  // giving reference to win condition to console
  temp_sdc->win_condition = &state.win_condition;

  // setting the temporary items tile
  temp_item_a.x = 3;
  temp_item_a.y = 7;
  temp_item_b.x = 12;
  temp_item_b.y = 5;
  temp_sdc->x = 15;
  temp_sdc->y = 13;
  // need to close the tile where the console is
  state.minimap.active_room->floor_tiles.map[13][15]->is_open = false;

  temp_item_a.index = temp_item_a.mesh_inx = 2;
  temp_item_b.index = temp_item_b.mesh_inx = 3;
  temp_sdc->index = temp_sdc->mesh_inx = 4;

  // TODO: will need to add the 3 interactibles to the entity system
  state.entities.allocate(3);

  // adding the temporary items to the active showcase room
  state.minimap.active_room->items.push_back(temp_item_a);
  state.minimap.active_room->items.push_back(temp_item_b);
  state.minimap.active_room->r_console = temp_sdc;

  lava::mesh_data cube_data = lava::create_mesh_data(lava::mesh_type::cube);
  lava::mesh::ptr cube_mesh = lava::make_mesh();
  lava::mesh::ptr cube_mesh2 = lava::make_mesh();
  cube_mesh->add_data(cube_data);
  cube_mesh->create(state.app->device);
  cube_data.scale(2);
  cube_data.move({0.f, -2.f, 0.f});
  cube_mesh2->add_data(cube_data);
  cube_mesh2->create(state.app->device);

  lava::mesh_data triangle_data =
      lava::create_mesh_data(lava::mesh_type::triangle);
  triangle_data.scale(4);
  lava::mesh::ptr triangle_mesh = lava::make_mesh();
  triangle_mesh->add_data(triangle_data);
  triangle_mesh->create(state.app->device);
  state.entities.meshes[temp_item_a.index] = cube_mesh;
  state.entities.initialize_transforms(*state.app, temp_item_a.index,
                                       state.desc_sets_list[2],
                                       state.descriptor_writes);
  state.entities.meshes[temp_item_b.index] = triangle_mesh;
  state.entities.initialize_transforms(*state.app, temp_item_b.index,
                                       state.desc_sets_list[3],
                                       state.descriptor_writes);
  state.entities.meshes[temp_sdc->index] = cube_mesh2;
  state.entities.initialize_transforms(*state.app, temp_sdc->index,
                                       state.desc_sets_list[4],
                                       state.descriptor_writes);

  // need to change the position of these items to match their tile
  glm::vec2 a_pos =
      state.minimap.active_room->get_tile_wpos(temp_item_a.x, temp_item_a.y);
  glm::vec2 b_pos =
      state.minimap.active_room->get_tile_wpos(temp_item_b.x, temp_item_b.y);
  glm::vec2 c_pos =
      state.minimap.active_room->get_tile_wpos(temp_sdc->x, temp_sdc->y);

  // setting the position of the interactibles
  state.entities.transforms_data[temp_item_a.index][3][0] = a_pos.x;
  state.entities.transforms_data[temp_item_a.index][3][2] = a_pos.y;

  state.entities.transforms_data[temp_item_b.index][3][0] = b_pos.x;
  state.entities.transforms_data[temp_item_b.index][3][2] = b_pos.y;

  state.entities.transforms_data[temp_sdc->index][3][0] = c_pos.x;
  state.entities.transforms_data[temp_sdc->index][3][2] = c_pos.y;

  // camera data
  state.camera_buffer_data = {
      state.app->camera.get_view_projection(),
  };

  // setting enemy manager
  state.enemy_manager.set_current_room(state.minimap.active_room);
  state.enemy_manager.load_entity_data(state.entities, crow::entity::SPHYNX,
                                       crow::entity::WORKER);
  state.enemy_manager.create_behavior_tree();

  // TODO: Move this from main (causes crash)
  /*state.camera_buffer.create_mapped(
      state.app->device, &state.camera_buffer_data,
      sizeof(state.camera_buffer_data),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);*/

  // Create entities
  // PLAYER CREATION
  std::string player_fbx_path =
      crow::get_exe_path() +
      "../../res/fbx/character.fbx";  // temp character model
  ofbx::IScene* scene_player =
      lava::extras::load_fbx_scene(player_fbx_path.c_str());
  fmt::print("Loaded FBX scene.\n");
  lava::extras::fbx_data player_fbx_data =
      lava::extras::load_fbx_model(scene_player);
  player_fbx_data.mesh_data.scale(0.05f);
  lava::mesh::ptr player_mesh = lava::make_mesh();
  player_mesh->add_data(player_fbx_data.mesh_data);
  player_mesh->create(state.app->device);
  state.entities.meshes[crow::entity::WORKER] = player_mesh;
  state.entities.initialize_transforms(
      *state.app, crow::entity::WORKER,
      state.desc_sets_list[crow::entity::WORKER], state.descriptor_writes);
  state.entities.velocities[crow::entity::WORKER] = glm::vec3{0, 0, 0};

  // ENEMY CREATION
  std::string enemy_fbx_path = crow::get_exe_path() + "../../res/fbx/deer.fbx";
  ofbx::IScene* scene_enemy =
      lava::extras::load_fbx_scene(enemy_fbx_path.c_str());
  fmt::print("Loaded FBX scene");
  lava::extras::fbx_data enemy_fbx_data =
      lava::extras::load_fbx_model(scene_enemy);
  enemy_fbx_data.mesh_data.scale(0.05f);
  lava::mesh::ptr enemy_mesh = lava::make_mesh();
  enemy_mesh->add_data(enemy_fbx_data.mesh_data);
  enemy_mesh->create(state.app->device);
  state.entities.meshes[crow::entity::SPHYNX] = enemy_mesh;
  state.entities.initialize_transforms(
      *state.app, crow::entity::SPHYNX,
      state.desc_sets_list[crow::entity::SPHYNX], state.descriptor_writes);
  state.entities.velocities[crow::entity::SPHYNX] = glm::vec3{0, 0, 0};
  crow::update_descriptor_writes(*state.app, state.descriptor_writes);

  state.left_click_time = 0;
  state.current_state = state.PLAYING;

  // FOR BUILD PURPOSES ONLY, TO BE REMOVED
  state.entities.transforms_data[crow::entity::WORKER][3][0] = -7.0f;
  state.entities.transforms_data[crow::entity::WORKER][3][2] = -7.0f;

  state.entities.transforms_data[crow::entity::SPHYNX][3][0] = 7.0f;
  state.entities.transforms_data[crow::entity::SPHYNX][3][2] = 7.0f;
}

void end_game(crow::game_state& state) {
  for (int i = 0; i < 1; i++) {
    state.entities.transforms_pbuffer[i].get()->destroy();
    state.entities.meshes[i].get()->destroy();
  }
}

// unused atm, should clean memory before starting a new game or exiting the
// program
void clean_state(crow::game_state& state) {}

void update(crow::game_state& state, lava::delta dt) {
  crow::path_through(state.player_data, state.entities, crow::entity::WORKER,
                     dt);

  // setting up enemy data for behavior tree (temporary)
  state.enemy_manager.set_current_room(state.minimap.active_room);

  state.enemy_manager.update_position(state.entities, crow::entity::SPHYNX);
  state.enemy_manager.update_target_position(state.entities,
                                             crow::entity::WORKER);
  // running behavior tree for ai
  status b_tree_result = state.enemy_manager.b_tree.run();

  for (size_t i = 0; i < state.entities.transforms_data.size(); i++) {
    state.entities.update_transform_data(i, dt);
    state.entities.update_transform_buffer(i);
  }

  if (state.current_state == state.PLAYING) {
    state.app->camera.update_view(dt, state.app->input.get_mouse_position());
    state.camera_buffer_data.projection_view =
        state.app->camera.get_view_projection();
    memcpy(state.camera_buffer.get_mapped_data(), &state.camera_buffer_data,
           sizeof(state.camera_buffer_data));

    // time elapsed since last left click
    state.left_click_time += dt;
    // updates sound timer objects
    crow::audio::update_audio_timers(&state, dt);
  }
}

auto left_click_update(game_state& state) -> bool {
  // processing for left clicks while you are currently playing the game
  if (state.current_state == state.PLAYING) {
    // crow::audio::play_sfx(0);
    glm::vec3 mouse_point = crow::mouse_to_floor(state.app);
    // if mouse_point.y == -1 then the mouse is not pointing at the
    // floor
    if (mouse_point.y != -1) {
      std::vector<glm::vec2> temporary_results =
          state.minimap.active_room->get_path(
              glm::vec2(
                  state.entities.transforms_data[crow::entity::WORKER][3][0],
                  state.entities.transforms_data[crow::entity::WORKER][3][2]),
              glm::vec2(mouse_point.x, mouse_point.z));

      if (temporary_results.size()) {
        // if the clicked position is the same as the previous position,
        // then we can assume that you've double clicked. thus, the
        // worker should run instead of walk
        if (state.player_data.path_result.size() &&
            state.player_data.path_result[0] == temporary_results[0]) {
          // check to ensure that the clicks were close enough to each
          // other to count as a double click. if not, then nothing
          // should happen since the worker is always walking towards
          // the clicked destination
          if (state.left_click_time < 0.5f) {
            // worker starts running to destination
            state.player_data.worker_speed = state.player_data.worker_run_speed;

            // plays footstep sound when worker moves
            crow::audio::add_footstep_sound(
                &state.entities.transforms_data[crow::WORKER], 0.285f);
          }
        } else {
          // worker starts walking to destination
          state.player_data.worker_speed = state.player_data.worker_walk_speed;

          // plays footstep sound when worker moves
          crow::audio::add_footstep_sound(
              &state.entities.transforms_data[crow::WORKER], 0.5f);
        }
      }

      // set the worker's path
      state.player_data.path_result = temporary_results;
    }
  }
  state.left_click_time = 0;

  return true;
}

auto right_click_update(game_state& state) -> bool {
  // processing for left clicks while you are currently playing the game
  if (state.current_state == state.PLAYING) {
    glm::vec3 mouse_point = crow::mouse_to_floor(state.app);
    // if mouse_point.y == -1 then the mouse is not pointing at the
    // floor
    if (mouse_point.y != -1) {
      const crow::tile* map_point = state.minimap.active_room->get_tile_at(
          {mouse_point.x, mouse_point.z});
      if (state.minimap.active_room->items.size() > 0) {
        for (item& i : state.minimap.active_room->items) {
          if (i.type != item_type::NONE && map_point->col == i.x &&
              map_point->row == i.y) {
            // if there is an item at this location we can then move to the item
            // and interact
            // setting interactible target for player
            state.player_data.item_target = &i;
            state.player_data.sdc_target = nullptr;
            std::vector<glm::vec2> temporary_results =
                state.minimap.active_room->get_path(
                    glm::vec2(state.entities
                                  .transforms_data[crow::entity::WORKER][3][0],
                              state.entities
                                  .transforms_data[crow::entity::WORKER][3][2]),
                    glm::vec2(mouse_point.x, mouse_point.z));

            if (temporary_results.size()) {
              // if the clicked position is the same as the previous position,
              // then we can assume that you've double clicked. thus, the
              // worker should run instead of walk
              if (state.player_data.path_result.size() &&
                  state.player_data.path_result[0] == temporary_results[0]) {
                // check to ensure that the clicks were close enough to each
                // other to count as a double click. if not, then nothing
                // should happen since the worker is always walking towards
                // the clicked destination
                if (state.right_click_time < 0.5f) {
                  // worker starts running to destination
                  state.player_data.worker_speed =
                      state.player_data.worker_run_speed;

                  // plays footstep sound when worker moves
                  crow::audio::add_footstep_sound(
                      &state.entities.transforms_data[crow::WORKER], 0.285f);
                }
              } else {
                // worker starts walking to destination
                state.player_data.worker_speed =
                    state.player_data.worker_walk_speed;

                // plays footstep sound when worker moves
                crow::audio::add_footstep_sound(
                    &state.entities.transforms_data[crow::WORKER], 0.5f);
              }
            }

            // set the worker's path
            state.player_data.path_result = temporary_results;
            state.player_data.interacting = true;
            break;
          }
        }
        state.right_click_time = 0;
      }
      if (state.minimap.active_room->r_console &&
          !state.minimap.active_room->r_console->active &&
          (map_point->col == state.minimap.active_room->r_console->x &&
           map_point->row == state.minimap.active_room->r_console->y)) {
        glm::vec2 c_to_p = {
            state.entities.transforms_data[crow::entity::WORKER][3][0] -
                mouse_point.x,
            state.entities.transforms_data[crow::entity::WORKER][3][2] -
                mouse_point.y};
        c_to_p = glm::normalize(c_to_p);
        c_to_p += glm::vec2{
            state.entities.transforms_data[state.minimap.active_room->r_console
                                               ->index][3][0],
            state.entities.transforms_data[state.minimap.active_room->r_console
                                               ->index][3][2]};

        state.player_data.sdc_target = state.minimap.active_room->r_console;
        state.player_data.item_target = nullptr;
        std::vector<glm::vec2> temporary_results =
            state.minimap.active_room->get_path(
                glm::vec2(
                    state.entities.transforms_data[crow::entity::WORKER][3][0],
                    state.entities.transforms_data[crow::entity::WORKER][3][2]),
                c_to_p);
        if (temporary_results.size()) {
          // if the clicked position is the same as the previous position,
          // then we can assume that you've double clicked. thus, the
          // worker should run instead of walk
          if (state.player_data.path_result.size() &&
              state.player_data.path_result[0] == temporary_results[0]) {
            // check to ensure that the clicks were close enough to each
            // other to count as a double click. if not, then nothing
            // should happen since the worker is always walking towards
            // the clicked destination
            if (state.right_click_time < 0.5f) {
              // worker starts running to destination
              state.player_data.worker_speed =
                  state.player_data.worker_run_speed;

              // plays footstep sound when worker moves
              crow::audio::add_footstep_sound(
                  &state.entities.transforms_data[crow::WORKER], 0.285f);
            }
          } else {
            // worker starts walking to destination
            state.player_data.worker_speed =
                state.player_data.worker_walk_speed;

            // plays footstep sound when worker moves
            crow::audio::add_footstep_sound(
                &state.entities.transforms_data[crow::WORKER], 0.5f);
          }
        }

        // set the worker's path
        state.player_data.path_result = temporary_results;
        state.player_data.interacting = true;
      }
    }
  }
  return true;
}

}  // namespace crow