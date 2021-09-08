#include "../hpp/game_state.hpp"

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
    }

    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.27f});
    if (ImGui::Button("Main Menu", pause_menu_button_wh)) {
      state.current_state = state.MAIN_MENU;
    }

    ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.35f});
    if (ImGui::Button("Quit", pause_menu_button_wh)) {
      // no, that button doesn't quit the menu, it quits the game, and it
      // doesn't even go "are you sure?", it's just like x_x
      state.app->shut_down();
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
    }

    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.75f});
    if (ImGui::Button("Options", mm_button_wh)) {
      /* currently does nothing. TODO: this */
    }

    ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.85f});
    if (ImGui::Button("Quit", mm_button_wh)) {
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
  state.minimap->map_minc = {-300, -300};
  state.minimap->map_maxc = {300, 300};
  state.minimap->screen_minr = {0.0f, 0.65f};
  state.minimap->screen_maxr = {0.4f, 0.35f};
  state.minimap->resolution = {1920, 1080};
  state.minimap->set_window_size(state.app->window.get_size());
  state.world_map.generate_blocks(4);
  state.world_map.generate_rooms(6, 3);
  state.world_map.generate_adjacencies();
  state.minimap->populate_map_data(&state.world_map);

  // if (!state.map_created) {
  // Create entities.
  lava::mesh::ptr player_mesh = lava::make_mesh();
  lava::mesh_data player_mesh_data =
      lava::create_mesh_data(lava::mesh_type::cube);
  state.entities->meshes[crow::entity::WORKER] = player_mesh_data;
  state.entities->initialize_transform(*state.app, crow::entity::WORKER);
  state.entities->velocities[crow::entity::WORKER] = glm::vec3{0.1f, 0, 0};
  //}
  state.map_created = true;

  state.current_state = state.PLAYING;
}

void end_game(crow::game_state& state) {
  for (int i = 0; i < 1; i++) {
    state.entities->transforms_pbuffer[i].get()->destroy();
  }
}
}  // namespace crow
