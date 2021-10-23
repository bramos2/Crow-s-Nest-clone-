#include <imgui.h>

#include "../hpp/audio.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
	void game_manager::imgui_on_draw() {
		ImGuiIO& io = ImGui::GetIO();
		// teach ImGui where the mouse is
        io.MousePos = { mouse_pos.x, mouse_pos.y };
		io.MouseDown[0] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
		io.MouseDown[1] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
		// this prevents the program from crashing
		io.DisplaySize.x = 1920.0f;
		io.DisplaySize.y = 1280.0f;
		io.WantCaptureMouse = false;

		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		// IMGUI SETUP FOR THIS FRAME IS COMPLETE, ALL IMGUI DRAWS GO IN HERE:
		ImVec2 wh = get_window_size();

        switch (current_state) {
        case game_state::PLAYING:
            minimap.draw_call(*this);
            break;
        }

		// this is just here to prove to you that yes, it does in fact work. it will be gone in the next commit.
		ImVec2 s = get_window_size();
		float3e p = mouse_to_floor(view, mouse_pos, s.x, s.y);
		ImGui::Text("clicked on : %f %f %f", p.x, p.y, p.z);
		ImGui::Text("mpos: %f, %f", mouse_pos.x, mouse_pos.y);
		ImGui::Text("wpos: %f, %f", wh.x, wh.y);
		if (ImGui::Button("click me")) audio::play_sfx(0);

		//draw_main_menu(wh);


		// THIS MUST COME AFTER ALL IMGUI DRAW CALLS
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

    void game_manager::draw_main_menu(ImVec2 wh) {
      // first things first, let's set the size of the main menu. it covers
      // the
      // whole screen tho, so...
      ImVec2 mm_window_xy = {0, 0};
      ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
      ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
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
      imgui_centertext(std::string("Crow's Nest"), 2.0f, wh);

      // size of all menu options
      ImVec2 mm_button_wh = {wh.x * 0.55f, wh.y * 0.05f};

      ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.45f});
      if (ImGui::Button("New Game", mm_button_wh)) {
        // end_game();
        new_game();
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.55f});
      if (ImGui::Button("Continue", mm_button_wh)) {
        // end_game();
        new_game();
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.65f});
      if (ImGui::Button("Options", mm_button_wh)) {
        crow::audio::play_sfx(crow::audio::MENU_OK);
        menu_position = 20;
      }

      ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.75f});
      if (ImGui::Button("Credits", mm_button_wh)) {
        /* currently does nothing. TODO: this */
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.85f});
      if (ImGui::Button("Quit", mm_button_wh)) {
        // no, that button doesn't quit the menu, it quits the game, and it
        // doesn't even go "are you sure?", it's just like x_x
        crow::audio::play_sfx(crow::audio::MENU_OK);
        PostQuitMessage(0);
      }
      ImGui::End();

      // no longer drawing borderless
      ImGui::PopStyleVar(1);
    }

    void game_manager::draw_pause_button(ImVec2 wh) {
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

    void game_manager::draw_pause_menu(ImVec2 wh) {  // set size parameters for the pause
                                            // window

      const int popup_flag = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoResize;
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
        current_state = game_state::PLAYING;
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.27f});
      if (ImGui::Button("Options", pause_menu_button_wh)) {
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::SetCursorPos({pause_window_wh.x * 0.225f, wh.y * 0.35f});
      if (ImGui::Button("Main Menu", pause_menu_button_wh)) {
        menu_position = 1;
        crow::audio::play_sfx(crow::audio::MENU_OK);
      }

      ImGui::End();

      if (menu_position == 1) {
        ImVec2 confirm_window_xy = {wh.x * 0.25f, wh.y * 0.375f};
        ImVec2 confirm_window_wh = {wh.x * 0.5f, wh.y * 0.25f};
        ImGui::SetNextWindowPos(confirm_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(confirm_window_wh, ImGuiCond_Always);
        ImGui::Begin("Return Confirm", 0, popup_flag);
        // confirmation dialog
        imgui_centertext(std::string("Quit to main menu?"), 2.0f, wh);
        ImGui::NewLine();
        imgui_centertext(std::string("Unsaved progress will be lost."), 2.0f, wh);

        ImVec2 confirm_button_wh = {confirm_window_wh.x * 0.25f, wh.y * 0.05f};
        ImGui::SetCursorPos(
            {confirm_window_wh.x * 0.15f, confirm_window_wh.y * 0.65f});
        if (ImGui::Button("Yes", confirm_button_wh)) {
          current_state = game_state::MAIN_MENU;
          crow::audio::play_sfx(crow::audio::MENU_OK);
          menu_position = 0;
          unload_game();
        }
        ImGui::SetCursorPos(
            {confirm_window_wh.x * 0.6f, confirm_window_wh.y * 0.65f});
        if (ImGui::Button("No", confirm_button_wh)) {
          crow::audio::play_sfx(crow::audio::MENU_OK);
          menu_position = 0;
        }

        ImGui::SetWindowFocus();
        ImGui::End();
      }
    }

    void game_manager::draw_game_over(ImVec2 wh) {
      // first things first, let's set the size of the main menu. it covers
      // the
      // whole screen tho, so...
      ImVec2 mm_window_xy = {0, 0};
      ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
      ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
      // black background so you can't tell that there is anything going on
      // behind it, borderless so you can't tell it's just an imgui window
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
      ImGui::SetNextWindowBgAlpha(1);
      // main menu drawing starts here
      ImGui::Begin("Game Over", 0,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoResize);
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();
      ImGui::NewLine();
      imgui_centertext(std::string("GAME OVER"), 8.0f, wh);
      ImGui::NewLine();
      imgui_centertext(std::string("The worker has died."), 2.0f, wh);
      ImGui::NewLine();

      if (state_time > 1) {
        // game over menu options
        ImVec2 game_over_button_wh = {wh.x * 0.15f, wh.y * 0.065f};
        ImGui::SetCursorPos({wh.x * 0.25f, wh.y * 0.65f});
        if (ImGui::Button("Retry", game_over_button_wh)) {
          crow::audio::play_sfx(crow::audio::MENU_OK);
          unload_game();
          current_state = game_state::MAIN_MENU;
          // TODO::selecting this option is the same as hitting the continue button
          // on the main menu
        }
        ImGui::SetCursorPos({wh.x * 0.6f, wh.y * 0.65f});
        if (ImGui::Button("Give up", game_over_button_wh)) {
          crow::audio::play_sfx(crow::audio::MENU_OK);
          PostQuitMessage(0);
        }
      }
      ImGui::End();
      ImGui::PopStyleVar(1);
    }

    void game_manager::draw_control_message(ImVec2 wh) {
      ImVec2 info_window_xy = {wh.x * 0.75f, wh.y * 0.65f};
      ImVec2 info_window_wh = {wh.x * 0.25f, wh.y * 0.35f};
      ImGui::SetNextWindowPos(info_window_xy, ImGuiCond_Always);
      ImGui::SetNextWindowSize(info_window_wh, ImGuiCond_Always);
      ImGui::Begin("control window", 0,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoResize);
      ImGui::SetWindowFontScale(2.0f);
      {
        std::string ptext = "Controls:";
        float text_size = ImGui::GetFontSize() * ptext.size() / 2;
        ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
                        (text_size / 2.0f));
        ImGui::Text(ptext.c_str());
      }
      ImGui::SetWindowFontScale(1.5f);
      ImGui::NewLine();
      {
        std::string ptext = "L click to move";
        float text_size = ImGui::GetFontSize() * ptext.size() / 2;
        ImGui::SameLine();
        ImGui::Text(ptext.c_str());
      }
      ImGui::NewLine();
      {
        std::string ptext = "R click to Interact";
        float text_size = ImGui::GetFontSize() * ptext.size() / 2;
        ImGui::SameLine();
        ImGui::Text(ptext.c_str());
      }
      ImGui::SetWindowFontScale(1.4f);
      ImGui::NewLine();
      ImGui::NewLine();
      {
        std::string ptext = "Click on minimap boxes";
        float text_size = ImGui::GetFontSize() * ptext.size() / 2;
        ImGui::SameLine();
        ImGui::Text(ptext.c_str());
      }
      ImGui::NewLine();
      {
        std::string ptext = "To switch room view";
        float text_size = ImGui::GetFontSize() * ptext.size() / 2;
        ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - text_size +
                        (text_size / 2.0f));
        ImGui::Text(ptext.c_str());
      }

      ImGui::End();
    }

    void game_manager::draw_oxygen_remaining(ImVec2 wh) {
      std::string text = "OXYGEN REMAINING";

      float scale = 2;

      float text_size =
          wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

      ImVec2 oxy_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.15f,
                              wh.y * 0.02f};
      ImVec2 oxy_window_wh = {text_size + wh.x * 0.3f,
                              wh.y * 0.048f + wh.y * 0.08f};
      ImGui::SetNextWindowPos(oxy_window_xy, ImGuiCond_Always);
      ImGui::SetNextWindowSize(oxy_window_wh, ImGuiCond_Always);
      ImGui::Begin("oxy_wind_p", 0,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoResize);

      ImGui::SetWindowFontScale(wh.x / 960.f * scale);
      ImGui::SetCursorPos({wh.x * 0.15f, wh.y * 0.04f});
      ImGui::Text(text.c_str());

      // draw the bar using a button, just because we can
      // TODO::load a texture instead of something as hilarious as a BUTTON
      // TODO::restore this to its former glory
      ImVec2 oxy_bar_wh = {            0,
          //(oxy_window_wh.x * 0.92f) * (current_level.selected_room->oxygen /
          //                             current_level.selected_room->oxygen_max),
          oxy_window_wh.y * 0.7f};
      // positioning
      ImGui::SetCursorPos(
          {oxy_window_wh.x * 0.04f, oxy_window_wh.y * 0.15f});  // positioning

      ImGui::Button("", oxy_bar_wh);

      ImGui::End();
    }

}  // namespace crow
