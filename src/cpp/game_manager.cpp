#include "../hpp/game_manager.hpp"

#include <liblava-extras/fbx.hpp>

#include "../hpp/audio.hpp"
#include "../hpp/cross.hpp"
#include "../hpp/game_objects.hpp"
#include "../hpp/player_behavior.hpp"
#include "../hpp/tile.hpp"

namespace crow {
game_manager::game_manager() {}

game_manager::game_manager(int _argc, char* _argv[]) {
  argc = _argc;
  argv = _argv;
}

game_manager::~game_manager() { cleanup(); }

void game_manager::new_game() {
  load_mesh_data();

  if (entities.current_size < 2) {
    entities.allocate(2);
  }
  entities.initialize_entity(
      app, static_cast<size_t>(entity::WORKER),
      mesh_models[static_cast<size_t>(crow::object_type::PLAYER)],
      &descriptor_writes, descriptor_pool, camera_buffer);
  entities.set_world_position(static_cast<size_t>(entity::WORKER), 0.f, 0.f,
                              0.f);

  entities.initialize_entity(
      app, static_cast<size_t>(entity::SPHYNX),
      mesh_models[static_cast<size_t>(crow::object_type::ENEMY)],
      &descriptor_writes, descriptor_pool, camera_buffer);
  entities.set_world_position(static_cast<size_t>(entity::SPHYNX), 0.f, 0.f,
                              0.f);

  current_level.load_level(app, 0);
  current_level.load_entities(app, entities, mesh_models, &descriptor_writes,
                              descriptor_pool, camera_buffer);

  // auto-load the first room
  current_level.select_default_room();
  crow::update_room_cam(current_level.selected_room->cam_pos,
                        current_level.selected_room->cam_rotation, app->camera);


  current_level.p_inter = &player_data.player_interact;
  // now crashing after new game more than once
  crow::update_descriptor_writes(app, &descriptor_writes);

  // setting up minimap
  minimap = crow::minimap({0.0f, 0.65f}, {0.4f, 0.35f});
  minimap.map_minc = {-300, -300};
  minimap.map_maxc = {300, 300};
  minimap.screen_minr = {0.0f, 0.65f};
  minimap.screen_maxr = {0.25f, 0.35f};
  minimap.resolution = {1920, 1080};
  minimap.set_window_size(app->window.get_size());
  minimap.current_level = &current_level;
  minimap.calculate_extents();

  // must always start on the starting room
  // player_data.current_room = current_level.starting_room;

  ai_m.init_manager(&entities, &current_level);
  ai_bt.aim = &ai_m;
  ai_bt.build_tree();

  current_state = crow::game_manager::game_state::PLAYING;
}

void game_manager::load_mesh_data() {
  auto get_fbx_mesh = [&](std::string fbx_path,
                          float scale_factor = 1.f) -> lava::mesh::ptr {
    std::string path = crow::get_exe_path() + fbx_path;
    ofbx::IScene* scene = lava::extras::load_fbx_scene(path.c_str());
    lava::extras::fbx_data fbx_data = lava::extras::load_fbx_model(scene);
    fbx_data.mesh_data.scale(scale_factor);
    lava::mesh::ptr fbx_mesh = lava::make_mesh();
    fbx_mesh->add_data(fbx_data.mesh_data);
    fbx_mesh->create(app->device);
    return fbx_mesh;
  };

  auto get_cube_mesh = [&](float x = 1.f, float y = 1.f,
                           float z = 1.f) -> lava::mesh::ptr {
    lava::mesh_data cube_data = lava::create_mesh_data(lava::mesh_type::cube);
    cube_data.scale_vector({x, y, z});
    lava::mesh::ptr cube_mesh = lava::make_mesh();
    cube_mesh->add_data(cube_data);
    cube_mesh->create(app->device);
    return cube_mesh;
  };

  // player mesh
  mesh_models.push_back(get_fbx_mesh("../../res/fbx/character.fbx", 0.05f));
  // enemy mesh
  mesh_models.push_back(get_fbx_mesh("../../res/fbx/deer.fbx", 0.05f));
  // sd console mesh
  mesh_models.push_back(get_cube_mesh(1.5f, 4.f, 1.5f));
  // power console mesh
  mesh_models.push_back(get_cube_mesh(2.f, 5.f, 2.f));
  // door mesh
  mesh_models.push_back(get_cube_mesh(0.5f, 10.f, 2.f));
  // exit door mesh
  mesh_models.push_back(get_cube_mesh(0.5f, 10.f, 3.5f));
  // door panel mesh
  mesh_models.push_back(get_cube_mesh(0.5f, 7.f, 1.8f));
}

void game_manager::unload_game() {
  // TODO: clean up entity system
  // TODO: anything that is initialized in new_game must be cleaned
  while (!descriptor_writes.empty()) {
    descriptor_writes.pop();
  }

  while (entities.current_size > 0) {
    entities.pop_back();
  }

  while (!mesh_models.empty()) {
    mesh_models_trash.push_back(mesh_models.back());
    mesh_models.pop_back();
  }

  current_level.clean_level(mesh_models_trash);

  ai_bt.clean_tree();
}

void game_manager::render_game() {
  environment_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
    if (current_level.selected_room && entities.current_size > 0) {
      for (size_t i = 0; i < current_level.selected_room->object_indices.size();
           ++i) {
        const size_t j = current_level.selected_room->object_indices[i];
        app->device->call().vkCmdBindDescriptorSets(
            cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
            environment_pipeline_layout->get(), 0, 4,
            entities.desc_sets_list[j].data(), 0, nullptr);
        entities.meshes[j]->bind_draw(cmd_buf);
      }
    }
  };
}

auto game_manager::l_click_update() -> bool {
  // processing for left clicks while you are currently playing the game
  if (current_state == crow::game_manager::game_state::PLAYING &&
      current_level.selected_room && current_level.selected_room->has_player) {
    // these next two lines prevents the player from moving when you click on
    // the minimap
    lava::mouse_position_ref _mouse_pos = app->input.get_mouse_position();
    if (!minimap.inside_minimap(_mouse_pos)) {
      // crow::audio::play_sfx(0);
      glm::vec3 mouse_point = crow::mouse_to_floor(app);
      // y = -1 out of bound
      if (mouse_point.y != -1) {
        const glm::vec3 player_pos = entities.get_world_position(
            static_cast<size_t>(crow::entity::WORKER));
        std::vector<glm::vec2> temporary_results =
            current_level.selected_room->get_path(
                glm::vec2(player_pos.x, player_pos.z),
                glm::vec2(mouse_point.x, mouse_point.z));

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
                  &entities.transforms_data[static_cast<size_t>(
                      crow::entity::WORKER)],
                  0.285f);
            }
          } else {
            // worker starts walking to destination
            player_data.worker_speed = player_data.worker_walk_speed;

            // plays footstep sound when worker moves
            crow::audio::add_footstep_sound(
                &entities.transforms_data[static_cast<size_t>(
                    crow::entity::WORKER)],
                0.5f);
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

auto game_manager::r_click_update() -> bool {
  crow::room* selected_room = current_level.selected_room;
  if (current_state == crow::game_manager::game_state::PLAYING &&
      selected_room && selected_room->has_player) {
    player_data.interacting = false;
    player_data.target = nullptr;

    glm::vec3 mouse_point = crow::mouse_to_floor(app);
    if (mouse_point.y == -1) {
      return true;
    }
    const crow::tile* clicked_tile =
        selected_room->get_tile_at(glm::vec2(mouse_point.x, mouse_point.z));

    if (!clicked_tile) {
      return true;
    }

    for (auto& i : selected_room->objects) {
      if (clicked_tile->row == i->y && clicked_tile->col == i->x) {
        player_data.interacting = true;
        player_data.target = i;

        const glm::vec3 p_pos = entities.get_world_position(
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

        glm::vec2 adjacent_tile =
            glm::vec2{static_cast<float>(p_tile->col) -
                          static_cast<float>(clicked_tile->col),
                      static_cast<float>(p_tile->row) -
                          static_cast<float>(clicked_tile->row)};

        adjacent_tile = glm::normalize(adjacent_tile);
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

        std::vector<glm::vec2> temporary_results =
            selected_room->get_path(glm::vec2(p_pos.x, p_pos.z), adjacent_tile);

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
                  &entities.transforms_data[static_cast<size_t>(
                      crow::entity::WORKER)],
                  0.285f);
            }
          } else {
            player_data.worker_speed = player_data.worker_walk_speed;

            crow::audio::add_footstep_sound(
                &entities.transforms_data[static_cast<size_t>(
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

void game_manager::cleanup() {
  unload_game();
  app->shut_down();
  delete app;
}

void game_manager::imgui_centertext(std::string text, float scale, ImVec2 wh) {
  ImGui::SetWindowFontScale(wh.x / 960.f * scale);
  float text_size = ImGui::GetFontSize() * text.size() / 2;
  ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
                  (text_size / 2.0f));
  ImGui::Text(text.c_str());
}

}  // namespace crow

//#include "../hpp/game_state.hpp"
//
//#include <liblava-extras/fbx.hpp>
//
//#include "../hpp/audio.hpp"
//#include "../hpp/camera.hpp"
//#include "../hpp/cross.hpp"
//#include "../hpp/interactible.hpp" 1
//
// namespace crow {
//
// void draw_menus(game_state& state, ImVec2 wh) {
//  // the game is paused, so draw a pause menu
//  if (state.current_state == state.PAUSED) {
//    // set size parameters for the pause window
//    ImVec2 pause_window_xy = {wh.x * 0.35f, wh.y * 0.275f};
//    ImVec2 pause_window_wh = {wh.x * 0.3f, wh.y * 0.45f};
//    ImGui::SetNextWindowPos(pause_window_xy, ImGuiCond_Always);
//    ImGui::SetNextWindowSize(pause_window_wh, ImGuiCond_Always);
//    ImGui::Begin("Pause Window", 0,
//                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
//                     ImGuiWindowFlags_NoResize);
//
//    // some text at the top of the window to let you know that the game is
//    // paused right now this is some text, we may want to change this to an
//    // image or somesuch later
//    std::string ptext = "PAUSED";
//    ImGui::SetWindowFontScale(wh.x / 960.f * 2.0f);
//    float text_size = ImGui::GetFontSize() * ptext.size() / 2;
//    ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
//                    (text_size / 2.0f));
//    ImGui::Text(ptext.c_str());
//
//    ImVec2 pause_menu_button_wh = {pause_window_wh.x * 0.55f, wh.y * 0.05f};
//
//    // TODO: the text buttons are placeholders, we will need proper image
//    // buttons later draw all the menu options here
//    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.19f});
//    if (ImGui::Button("Resume", pause_menu_button_wh)) {
//      state.current_state = state.PLAYING;
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//    }
//
//    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.27f});
//    if (ImGui::Button("Main Menu", pause_menu_button_wh)) {
//      state.current_state = state.MAIN_MENU;
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//    }
//
//    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.35f});
//    if (ImGui::Button("Quit", pause_menu_button_wh)) {
//      // no, that button doesn't quit the menu, it quits the game, and it
//      // doesn't even go "are you sure?", it's just like x_x
//      state.app->shut_down();
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//    }
//
//    ImGui::End();
//  }
//
//  // the game is MAIN MENU7, so draw the main menu
//  if (state.current_state == state.MAIN_MENU) {
//    // first things first, let's set the size of the main menu. it covers the
//    // whole screen tho, so...
//    ImVec2 mm_window_xy = {0, 0};
//    ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
//    ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
//    // black background so you can't tell that there is anything going on
//    behind
//    // it, borderless so you can't tell it's just an imgui window
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
//    ImGui::SetNextWindowBgAlpha(1);
//    // main menu drawing starts here
//    ImGui::Begin("Main Menu", 0,
//                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
//                     ImGuiWindowFlags_NoResize);
//
//    // as a placeholder, i have the game name as some tedext at the top, there
//    // should probably be a logo or something here later on (TODO)
//    std::string ptext = "Crow's Nest";
//    ImGui::SetWindowFontScale(wh.x / 960.f * 2.0f);
//    float text_size = ImGui::GetFontSize() * ptext.size() / 2;
//    ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
//                    (text_size / 2.0f));
//    ImGui::SetCursorPosY(wh.y * 0.16f);
//    ImGui::Text(ptext.c_str());
//
//    // size of all menu options
//    ImVec2 mm_button_wh = {wh.x * 0.55f, wh.y * 0.05f};
//
//    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.65f});
//    if (ImGui::Button("New Game", mm_button_wh)) {
//      end_game(state);
//      new_game(state);
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//    }
//
//    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.75f});
//    if (ImGui::Button("Options", mm_button_wh)) {
//      /* currently does nothing. TODO: this */
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//    }
//
//    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.85f});
//    if (ImGui::Button("Quit", mm_button_wh)) {
//      crow::audio::play_sfx(crow::audio::MENU_OK);
//      state.app->shut_down();
//    }
//    ImGui::End();
//
//    // no longer drawing borderless
//    ImGui::PopStyleVar(1);
//  }
//}
//
// void new_game(crow::game_state& state) {
//  // need to create/load all the levels first
//  // then we need to load the current level data into the minimap
//
//  //-----map  generation testing----
//  state.world_map = crow::world_map<5, 5>();
//  // minimap logic
//  state.minimap = crow::minimap({0.0f, 0.65f}, {0.4f, 0.35f});
//  state.minimap.map_minc = {-300, -300};
//  state.minimap.map_maxc = {300, 300};
//  state.minimap.screen_minr = {0.0f, 0.65f};
//  state.minimap.screen_maxr = {0.4f, 0.35f};
//  state.minimap.resolution = {1920, 1080};
//  state.minimap.set_window_size(state.app->window.get_size());
//  state.world_map.generate_blocks(4);
//  state.world_map.generate_rooms(6, 3);
//  state.world_map.generate_adjacencies();
//  state.minimap.populate_map_data(&state.world_map, state.app);
//  state.active_room = state.minimap.active_room;
//
//  // TEMPORARY
//  // TODO: ADD ITEMS TO SHOWCASE ROOM
//
//  interactible* inter_a = new interactible;
//  sd_console* sdc = new sd_console;
//  pg_console* pgc = new pg_console;
//
//  inter_a->index = 2;
//  sdc->index = 3;
//  pgc->index = 4;
//
//  // need to assign interactibles to a tile
//  inter_a->set_tile(3, 7);
//  sdc->set_tile(12, 5);
//  pgc->set_tile(15, 13);
//
//  // need to close the tiles where the consoles are located at
//  state.active_room->floor_tiles.map[inter_a->y][inter_a->x]->is_open = false;
//  state.active_room->floor_tiles.map[sdc->y][sdc->x]->is_open = false;
//  state.active_room->floor_tiles.map[pgc->y][pgc->x]->is_open = false;
//
//  state.active_room->consoles.push_back(inter_a);
//  state.active_room->consoles.push_back(sdc);
//  state.active_room->consoles.push_back(pgc);
//
//  lava::mesh_data cube_data = lava::create_mesh_data(lava::mesh_type::cube);
//  cube_data.scale(2);
//  lava::mesh::ptr cube_mesh = lava::make_mesh();
//  lava::mesh::ptr cube_mesh2 = lava::make_mesh();
//  lava::mesh::ptr cube_mesh3 = lava::make_mesh();
//
//  cube_mesh->add_data(cube_data);
//  cube_mesh->create(state.app->device);
//  cube_mesh2->add_data(cube_data);
//  cube_mesh2->create(state.app->device);
//  cube_mesh3->add_data(cube_data);
//  cube_mesh3->create(state.app->device);
//
//  state.entities.meshes[inter_a->index] = cube_mesh;
//  state.entities.initialize_transforms(*state.app, inter_a->index,
//                                       &state.entities.desc_sets_list[2],
//                                       state.descriptor_writes);
//
//  state.entities.meshes[sdc->index] = cube_mesh2;
//  state.entities.initialize_transforms(*state.app, sdc->index,
//                                       &state.entities.desc_sets_list[3],
//                                       state.descriptor_writes);
//
//  state.entities.meshes[pgc->index] = cube_mesh3;
//  state.entities.initialize_transforms(*state.app, pgc->index,
//                                       &state.entities.desc_sets_list[4],
//                                       state.descriptor_writes);
//
//  // need to change the position of these items to match their tile
//  glm::vec2 a_pos =
//      state.minimap.active_room->get_tile_wpos(inter_a->x, inter_a->y);
//  glm::vec2 b_pos = state.minimap.active_room->get_tile_wpos(sdc->x, sdc->y);
//  glm::vec2 c_pos = state.minimap.active_room->get_tile_wpos(pgc->x, pgc->y);
//
//  // setting the position of the interactibles
//  state.entities.transforms_data[inter_a->index][3][0] = a_pos.x;
//  state.entities.transforms_data[inter_a->index][3][2] = a_pos.y;
//
//  state.entities.transforms_data[sdc->index][3][0] = b_pos.x;
//  state.entities.transforms_data[sdc->index][3][2] = b_pos.y;
//  state.entities.transforms_data[pgc->index][3][0] = c_pos.x;
//  state.entities.transforms_data[pgc->index][3][2] = c_pos.y;
//
//  // need to make a way to load/unload items into the entity system based on
//  // active room
//  // TODO: load interactibles in entity system
//
//  /*item temp_item_a;
//  item temp_item_b;*/
//  // sd_console* temp_sdc = new sd_console;
//  /*temp_item_a.type = item_type::FLASK;
//  temp_item_b.type = item_type::ALARM;
//  temp_item_a.player_inv = temp_item_b.player_inv =
//      &state.player_data.player_inventory;*/
//
//  // giving reference to win condition to console
//  // temp_sdc->win_condition = &state.win_condition;
//
//  // setting the temporary items tile
//  /*temp_item_a.x = 3;
//  temp_item_a.y = 7;
//  temp_item_b.x = 12;
//  temp_item_b.y = 5;
//  temp_sdc->x = 15;
//  temp_sdc->y = 13;*/
//  // need to close the tile where the console is
//  // state.minimap.active_room->floor_tiles.map[13][15]->is_open = false;
//
//  /* temp_item_a.index = temp_item_a.mesh_inx = 2;
//   temp_item_b.index = temp_item_b.mesh_inx = 3;*/
//  // temp_sdc->index = temp_sdc->mesh_inx = 2;
//
//  // TODO: will need to add the 3 interactibles to the entity system
//  // state.entities.allocate(1);
//
//  // adding the temporary items to the active showcase room
//  /*state.minimap.active_room->items.push_back(temp_item_a);
//  state.minimap.active_room->items.push_back(temp_item_b);*/
//  // state.minimap.active_room->r_console = temp_sdc;
//
//  /* lava::mesh_data cube_data =
//  lava::create_mesh_data(lava::mesh_type::cube);
//   lava::mesh::ptr cube_mesh = lava::make_mesh();
//   lava::mesh::ptr cube_mesh2 = lava::make_mesh();*/
//  /*cube_mesh->add_data(cube_data);
//  cube_mesh->create(state.app->device);
//  cube_data.scale(2);
//  cube_data.move({0.f, -2.f, 0.f});
//  cube_mesh2->add_data(cube_data);
//  cube_mesh2->create(state.app->device);*/
//
//  /*lava::mesh_data triangle_data =
//      lava::create_mesh_data(lava::mesh_type::triangle);
//  triangle_data.scale(4);
//  lava::mesh::ptr triangle_mesh = lava::make_mesh();
//  triangle_mesh->add_data(triangle_data);
//  triangle_mesh->create(state.app->device);*/
//  // state.entities.meshes[temp_item_a.index] = cube_mesh;
//  /*state.entities.initialize_transforms(*state.app, temp_item_a.index,
//                                       state.desc_sets_list[2],
//                                       state.descriptor_writes);*/
//  /*state.entities.meshes[temp_item_b.index] = triangle_mesh;
//  state.entities.initialize_transforms(*state.app, temp_item_b.index,
//                                       state.desc_sets_list[3],
//                                       state.descriptor_writes);*/
//  // state.entities.meshes[temp_sdc->index] = cube_mesh2;
//  /*state.entities.initialize_transforms(*state.app, temp_sdc->index,
//                                       state.desc_sets_list[4],
//                                       state.descriptor_writes);*/
//
//  // need to change the position of these items to match their tile
//  /* glm::vec2 a_pos =
//       state.minimap.active_room->get_tile_wpos(temp_item_a.x, temp_item_a.y);
//   glm::vec2 b_pos =
//       state.minimap.active_room->get_tile_wpos(temp_item_b.x,
//       temp_item_b.y);*/
//  /*glm::vec2 c_pos =
//      state.minimap.active_room->get_tile_wpos(temp_sdc->x, temp_sdc->y);*/
//
//  // setting the position of the interactibles
//  /*state.entities.transforms_data[temp_item_a.index][3][0] = a_pos.x;
//  state.entities.transforms_data[temp_item_a.index][3][2] = a_pos.y;
//
//  state.entities.transforms_data[temp_item_b.index][3][0] = b_pos.x;
//  state.entities.transforms_data[temp_item_b.index][3][2] = b_pos.y;*/
//  // state.entities.transforms_data[temp_sdc->index][3][0] = c_pos.x;
//  // state.entities.transforms_data[temp_sdc->index][3][2] = c_pos.y;
//
//  // camera data
//  state.camera_buffer_data = {
//      state.app->camera.get_view_projection(),
//  };
//
//  // setting enemy manager
//  //state.enemy_manager.set_current_room(state.minimap.active_room);
//  //state.enemy_manager.load_entity_data(state.entities, crow::entity::SPHYNX,
//   //                                    crow::entity::WORKER);
//  //state.enemy_manager.create_behavior_tree();
//
//  // TODO: Move this from main (causes crash)
//  /*state.camera_buffer.create_mapped(
//      state.app->device, &state.camera_buffer_data,
//      sizeof(state.camera_buffer_data),
//      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
//      VK_BUFFER_USAGE_TRANSFER_DST_BIT);*/
//
//  // Create entities
//  // PLAYER CREATION
//  std::string player_fbx_path =
//      crow::get_exe_path() +
//      "../../res/fbx/character.fbx";  // temp character model
//  ofbx::IScene* scene_player =
//      lava::extras::load_fbx_scene(player_fbx_path.c_str());
//  fmt::print("Loaded FBX scene.\n");
//  lava::extras::fbx_data player_fbx_data =
//      lava::extras::load_fbx_model(scene_player);
//  player_fbx_data.mesh_data.scale(0.05f);
//  lava::mesh::ptr player_mesh = lava::make_mesh();
//  player_mesh->add_data(player_fbx_data.mesh_data);
//  player_mesh->create(state.app->device);
//  state.entities.meshes[crow::entity::WORKER] = player_mesh;
//  state.entities.initialize_transforms(
//      *state.app, crow::entity::WORKER,
//      &state.entities.desc_sets_list[crow::entity::WORKER],
//      state.descriptor_writes);
//  state.entities.velocities[crow::entity::WORKER] = glm::vec3{0, 0, 0};
//
//  // ENEMY CREATION
//  std::string enemy_fbx_path = crow::get_exe_path() +
//  "../../res/fbx/deer.fbx"; ofbx::IScene* scene_enemy =
//      lava::extras::load_fbx_scene(enemy_fbx_path.c_str());
//  fmt::print("Loaded FBX scene");
//  lava::extras::fbx_data enemy_fbx_data =
//      lava::extras::load_fbx_model(scene_enemy);
//  enemy_fbx_data.mesh_data.scale(0.05f);
//  lava::mesh::ptr enemy_mesh = lava::make_mesh();
//  enemy_mesh->add_data(enemy_fbx_data.mesh_data);
//  enemy_mesh->create(state.app->device);
//  state.entities.meshes[crow::entity::SPHYNX] = enemy_mesh;
//  state.entities.initialize_transforms(
//      *state.app, crow::entity::SPHYNX,
//      &state.entities.desc_sets_list[crow::entity::SPHYNX],
//      state.descriptor_writes);
//  state.entities.velocities[crow::entity::SPHYNX] = glm::vec3{0, 0, 0};
//
//  crow::update_descriptor_writes(state.app, state.descriptor_writes);
//
//  state.left_click_time = 0;
//  state.current_state = state.PLAYING;
//
//  // FOR BUILD PURPOSES ONLY, TO BE REMOVED
//  state.entities.transforms_data[crow::entity::WORKER][3][0] = -7.0f;
//  state.entities.transforms_data[crow::entity::WORKER][3][2] = -7.0f;
//
//  state.entities.transforms_data[crow::entity::SPHYNX][3][0] = 7.0f;
//  state.entities.transforms_data[crow::entity::SPHYNX][3][2] = 7.0f;
//}
//
// void end_game(crow::game_state& state) {
//  for (int i = 0; i < 1; i++) {
//    state.entities.transforms_pbuffer[i].get()->destroy();
//    state.entities.meshes[i].get()->destroy();
//  }
//}
//
//// unused atm, should clean memory before starting a new game or exiting the
//// program
// void clean_state(crow::game_state& state) {}
//
// void update(crow::game_state& state, lava::delta dt) {
//  crow::path_through(state.player_data, state.entities, crow::entity::WORKER,
//                     dt);
//
//  // setting up enemy data for behavior tree (temporary)
//  //state.enemy_manager.set_current_room(state.minimap.active_room);
//
//  //state.enemy_manager.update_position(state.entities);
//  //state.enemy_manager.update_target_position(state.entities,
//  //                                           crow::entity::WORKER);
//  // running behavior tree for ai
//  status b_tree_result = state.enemy_manager.b_tree.run();
//
//  for (size_t i = 0; i < state.entities.transforms_data.size(); i++) {
//    state.entities.update_transform_data(i, dt);
//    state.entities.update_transform_buffer(i);
//  }
//
//  if (state.current_state == state.PLAYING) {
//    state.app->camera.update_view(dt, state.app->input.get_mouse_position());
//    state.camera_buffer_data.projection_view =
//        state.app->camera.get_view_projection();
//    memcpy(state.camera_buffer.get_mapped_data(), &state.camera_buffer_data,
//           sizeof(state.camera_buffer_data));
//
//    // time elapsed since last left click
//    state.left_click_time += dt;
//    // updates sound timer objects
//    crow::audio::update_audio_timers(&state, dt);
//  }
//}
//
// auto left_click_update(game_state& state) -> bool {
//  // processing for left clicks while you are currently playing the game
//  if (state.current_state == state.PLAYING) {
//    // crow::audio::play_sfx(0);
//    glm::vec3 mouse_point = crow::mouse_to_floor(state.app);
//    // if mouse_point.y == -1 then the mouse is not pointing at the
//    // floor
//    if (mouse_point.y != -1) {
//      std::vector<glm::vec2> temporary_results =
//          state.minimap.active_room->get_path(
//              glm::vec2(
//                  state.entities.transforms_data[crow::entity::WORKER][3][0],
//                  state.entities.transforms_data[crow::entity::WORKER][3][2]),
//              glm::vec2(mouse_point.x, mouse_point.z));
//
//      if (temporary_results.size()) {
//        // if the clicked position is the same as the previous position,
//        // then we can assume that you've double clicked. thus, the
//        // worker should run instead of walk
//        if (state.player_data.path_result.size() &&
//            state.player_data.path_result[0] == temporary_results[0]) {
//          // check to ensure that the clicks were close enough to each
//          // other to count as a double click. if not, then nothing
//          // should happen since the worker is always walking towards
//          // the clicked destination
//          if (state.left_click_time < 0.5f) {
//            // worker starts running to destination
//            state.player_data.worker_speed =
//            state.player_data.worker_run_speed;
//
//            // plays footstep sound when worker moves
//            crow::audio::add_footstep_sound(
//                &state.entities.transforms_data[crow::WORKER], 0.285f);
//          }
//        } else {
//          // worker starts walking to destination
//          state.player_data.worker_speed =
//          state.player_data.worker_walk_speed;
//
//          // plays footstep sound when worker moves
//          crow::audio::add_footstep_sound(
//              &state.entities.transforms_data[crow::WORKER], 0.5f);
//        }
//      }
//
//      // set the worker's path
//      state.player_data.path_result = temporary_results;
//
//      state.player_data.interacting = false;
//      state.player_data.target = nullptr;
//    }
//  }
//  state.left_click_time = 0;
//
//  return true;
//}
//
// we are here
// auto right_click_update(game_state& state) -> bool {
//  // processing for left clicks while you are currently playing the game
//  if (state.current_state == state.PLAYING) {
//    glm::vec3 mouse_point = crow::mouse_to_floor(state.app);
//    // if mouse_point.y == -1 then the mouse is not pointing at the
//    // floor
//    if (mouse_point.y != -1) {
//      const crow::tile* map_point = state.minimap.active_room->get_tile_at(
//          {mouse_point.x, mouse_point.z});
//      if (state.active_room->consoles.size() > 0) {
//        for (interactible* i : state.active_room->consoles) {
//          if (!i->is_active && map_point->col == i->x && map_point->row ==
//          i->y) {
//            // if there is an item at this location we can then move to the
//            // item and interact
//
//            // setting interactible target for player
//            state.player_data.target = i;
//            const crow::tile* p_tile = state.active_room->get_tile_at(
//                {state.entities.transforms_data[entity::WORKER][3][0],
//                 state.entities.transforms_data[entity::WORKER][3][3]});
//
//            glm::vec2 adjacent_tile =
//            glm::vec2{static_cast<float>(p_tile->col) -
//            static_cast<float>(map_point->col),
//                                                static_cast<float>(p_tile->row)
//                                                -
//                                                static_cast<float>(map_point->row)};
//            adjacent_tile = glm::normalize(adjacent_tile);
//            for (size_t i = 0; i < 2; i++) {
//              if (adjacent_tile[i] >= 0.5f) {
//                adjacent_tile[i] = 1.f;
//                continue;
//              } else if (adjacent_tile[i] <= -0.5f) {
//                adjacent_tile[i] = -1.f;
//              }
//            }
//
//            adjacent_tile = {adjacent_tile.x +
//            static_cast<float>(map_point->col),
//                             adjacent_tile.y +
//                             static_cast<float>(map_point->row)};
//
//            adjacent_tile =
//            state.active_room->get_tile_wpos(static_cast<int>(adjacent_tile.x),
//                                                             static_cast<int>(adjacent_tile.y));
//
//            std::vector<glm::vec2> temporary_results =
//                state.minimap.active_room->get_path(
//                    glm::vec2(state.entities
//                                  .transforms_data[crow::entity::WORKER][3][0],
//                              state.entities
//                                  .transforms_data[crow::entity::WORKER][3][2]),
//                    adjacent_tile);
//
//            if (temporary_results.size()) {
//              // if the clicked position is the same as the previous position,
//              // then we can assume that you've double clicked. thus, the
//              // worker should run instead of walk
//              if (state.player_data.path_result.size() &&
//                  state.player_data.path_result[0] == temporary_results[0]) {
//                // check to ensure that the clicks were close enough to each
//                // other to count as a double click. if not, then nothing
//                // should happen since the worker is always walking towards
//                // the clicked destination
//                if (state.right_click_time < 0.5f) {
//                  // worker starts running to destination
//                  state.player_data.worker_speed =
//                      state.player_data.worker_run_speed;
//
//                  // plays footstep sound when worker moves
//                  crow::audio::add_footstep_sound(
//                      &state.entities.transforms_data[crow::WORKER], 0.285f);
//                }
//              } else {
//                // worker starts walking to destination
//                state.player_data.worker_speed =
//                    state.player_data.worker_walk_speed;
//
//                // plays footstep sound when worker moves
//                crow::audio::add_footstep_sound(
//                    &state.entities.transforms_data[crow::WORKER], 0.5f);
//              }
//            }
//
//            // set the worker's path
//            state.player_data.path_result = temporary_results;
//            state.player_data.interacting = true;
//            break;
//          }
//        }
//        state.right_click_time = 0;
//      }
//    }
//  }
//
//  return true;
//}
//
//}  // namespace crow