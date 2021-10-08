#include <imgui.h>

#include "../hpp/audio.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {

void game_manager::draw_main_menu() {
  glm::vec2 wh = app->window.get_size();
  // first things first, let's set the size of the main menu. it covers
  // the
  // whole screen tho, so...
  ImVec2 mm_window_xy = {0, 0};
  ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2{wh.x, wh.y}, ImGuiCond_Always);
  // black background so you can't tell that there is anything going on
  // behind it, borderless so you can't tell it's just an imgui window
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
  ImGui::SetNextWindowBgAlpha(1);
  // main menu drawing starts here
  ImGui::Begin("Main Menu", 0,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize);

  // as a placeholder, i have the game name as some tedext at the top,
  // there should probably be a logo or something here later on (TODO)
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
    new_game();
    // end_game(state);
    // new_game(state);
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
    app->shut_down();
  }
  ImGui::End();

  // no longer drawing borderless
  ImGui::PopStyleVar(1);
}

void game_manager::draw_pause_button() {
  // game_state.minimap.camera = &app.camera;
  // need this for having the GUI items scale with the window size
  glm::vec2 wh = app->window.get_size();
  // pass this flag into ImGui::Begin when you need to spawn a window
  // that / only contains a texture
  const int texture_flag =
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;

  // remove formatting for GUI windows to draw plain textures as GUI
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
  // set size parameters for the pause button icon
  ImVec2 pause_button_xy = {wh.x - (0.0333333f * wh.x), 0};
  ImVec2 pause_button_wh = {0.0333333f * wh.x, 0.0592592592592593f * wh.y};
  ImGui::SetNextWindowPos(pause_button_xy, ImGuiCond_Always);
  ImGui::SetNextWindowSize(pause_button_wh, ImGuiCond_Always);
  // finally create the pause button
  ImGui::Begin("Pause", nullptr, texture_flag);
  if (ImGui::ImageButton(nullptr /* INSERT TEXTURE POINTER HERE */,
                         pause_button_wh)) {
    crow::audio::play_sfx(crow::audio::MENU_OK);
    current_state = crow::game_manager::game_state::PAUSED;
  }

  ImGui::End();
  //// all texture-only GUI items should be before this line as it resets
  /// the / GUI window styling back to default
  ImGui::PopStyleVar(3);
}

void game_manager::draw_pause_menu() {  // set size parameters for the pause
                                        // window
  glm::vec2 wh = app->window.get_size();
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
    current_state = crow::game_manager::game_state::PLAYING;
    crow::audio::play_sfx(crow::audio::MENU_OK);
  }

  ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.27f});
  if (ImGui::Button("Main Menu", pause_menu_button_wh)) {
    current_state = crow::game_manager::game_state::MAIN_MENU;
    unload_game();
    crow::audio::play_sfx(crow::audio::MENU_OK);
  }

  ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.35f});
  if (ImGui::Button("Quit", pause_menu_button_wh)) {
    // no, that button doesn't quit the menu, it quits the game, and it
    // doesn't even go "are you sure?", it's just like x_x
    app->shut_down();
    crow::audio::play_sfx(crow::audio::MENU_OK);
  }

  ImGui::End();
}

}  // namespace crow
