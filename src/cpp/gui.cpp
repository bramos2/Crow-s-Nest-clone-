#include <imgui.h>

#include "../hpp/audio.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
    void game_manager::imgui_on_draw() {
        // actual size of the window cuz imgui is weirdo
        ImVec2 real_size = get_window_size();

        ImGuiIO& io = ImGui::GetIO();
        // teach ImGui where the mouse is
        mouse_pos_gui = { mouse_pos.x * (imgui_wsize.x / real_size.x), mouse_pos.y* (imgui_wsize.y / real_size.y) };
        io.MousePos = { mouse_pos_gui.x, mouse_pos_gui.y };
		io.MouseDown[0] = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
		io.MouseDown[1] = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;
		io.WantCaptureMouse = false;
		// this prevents the program from crashing, i dont know if it actually does anything though
        io.DisplaySize.x = real_size.x;
		io.DisplaySize.y = real_size.y;

		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();
		// IMGUI SETUP FOR THIS FRAME IS COMPLETE, ALL IMGUI DRAWS GO IN HERE:
		ImVec2 wh = imgui_wsize;

        switch (current_state) {
        case game_state::MAIN_MENU:
            draw_main_menu(wh);
            break;
        case game_state::PLAYING:
            minimap.draw_call(*this);
            current_message.display(1, wh);
            draw_pause_button(wh);
            draw_control_message(wh);
            draw_oxygen_remaining(wh);
            draw_pressure_remaining(wh);
            break;
        case game_state::PAUSED:
            draw_pause_menu(wh);
            break;
        case game_state::SETTINGS:
            draw_options_menu(wh);
            break;
        case game_state::GAME_OVER:
            draw_game_over(wh);
            break;
        }

        if (debug_mode) {
            ImVec2 s = get_window_size();
            float3e p = mouse_to_floor(view, mouse_pos, s.x, s.y);
            ImGui::Text("current level: %i: current room: %i", level_number, (imgui_wsize.y / real_size.y));
            ImGui::SameLine();
            if (current_level.pressure_console) {
                ImGui::Text("\tpressure: %f", current_level.pressure_console->pressure);
            }

            ImGui::Text("clicked on : %f %f %f", p.x, p.y, p.z);
            ImGui::Text("mpos: %f, %f", mouse_pos.x, mouse_pos.y);
            ImGui::Text("wpos: %f, %f", wh.x, wh.y);
            ImGui::Text("dpos: %f, %f", real_size.x, real_size.y);
            ImGui::Text("mpos: %f, %f", (imgui_wsize.x / real_size.x), (imgui_wsize.y / real_size.y));
            ImGui::Text("mousedrag: %i", ImGui::IsMouseDragging(ImGuiMouseButton_Left));
            ImGui::NewLine();
            ImGui::Text("mousedrag: %f, %f", minimap.mouse_position.x, minimap.mouse_position.y);
            ImGui::Text("mousewheel: %i", mwheel_delta);
            ImGui::SameLine();
            ImGui::Text("   rclick: %i", io.MouseDown[1]);
            DirectX::XMFLOAT3 worker_pos = entities.get_world_position(crow::entity::WORKER);
            ImGui::Text("worker: %f, %f", worker_pos.x, worker_pos.z);
		    if (ImGui::Button("click me")) audio::play_sfx(0);
        }

		// THIS MUST COME AFTER ALL IMGUI DRAW CALLS
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

    void game_manager::draw_main_menu(ImVec2 wh) {
        const int popup_flag = ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoResize;

        // first things first, let's set the size of the main menu. it covers
        // the
        // whole screen tho, so...
        ImVec2 mm_window_xy = {0, 0};
        ImGui::SetNextWindowPos(mm_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        // main menu drawing starts here
        ImGui::Begin("Main Menu", 0,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize);

        // as a placeholder, i have the game name as some tedext at the top,
        // there should probably be a logo or something here later on (TODO::)
        imgui_centertext(std::string("Crow's Nest"), 4.0f, wh);

        // size of all menu options
        ImVec2 mm_button_wh = {wh.x * 0.55f, wh.y * 0.05f};

        ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.45f});
        if (ImGui::Button("New Game", mm_button_wh)) {
            if (menu_position == 0) {
                crow::audio::play_sfx(crow::audio::MENU_OK);
                if (level_number != 0) {
                    menu_position = 3;
                } else {
                    level_number = 1;
                    new_game();
                    save_game();
                }
            }
        }

        ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.55f});
        if (ImGui::Button("Continue", mm_button_wh)) {
            if (menu_position == 0) {
                crow::audio::play_sfx(crow::audio::MENU_OK);
                new_game();
            }
        }

        ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.65f});
        if (ImGui::Button("Options", mm_button_wh)) {
            if (menu_position == 0) {
                crow::audio::play_sfx(crow::audio::MENU_OK);
                menu_position = 20;
                current_state = game_state::SETTINGS;
            }
        }

        ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.75f});
        if (ImGui::Button("Credits", mm_button_wh)) {
            if (menu_position == 0) {
                /* currently does nothing. TODO: this */
                crow::audio::play_sfx(crow::audio::MENU_OK);
            }
        }

        ImGui::SetCursorPos({wh.x * 0.225f, wh.y * 0.85f});
        if (ImGui::Button("Quit", mm_button_wh)) {
            if (menu_position == 0) {
                // no, that button doesn't quit the menu, it quits the game, and it
                // doesn't even go "are you sure?", it's just like x_x
                crow::audio::play_sfx(crow::audio::MENU_OK);
                PostQuitMessage(0);
            }
        }
        ImGui::End();

        // no longer drawing borderless
        ImGui::PopStyleVar(1);

        if (menu_position == 3) {
            ImVec2 confirm_window_xy = {wh.x * 0.25f, wh.y * 0.375f};
            ImVec2 confirm_window_wh = {wh.x * 0.5f, wh.y * 0.25f};
            ImGui::SetNextWindowPos(confirm_window_xy, ImGuiCond_Always);
            ImGui::SetNextWindowSize(confirm_window_wh, ImGuiCond_Always);
            ImGui::Begin("New Game Confirm", 0, popup_flag);
            // confirmation dialog
            imgui_centertext(std::string("Overwrite save data?"), 4.0f, confirm_window_wh);
            ImGui::NewLine();
            imgui_centertext(std::string("If you start a new game you will lose your progress."), 2.0f, confirm_window_wh);

            ImVec2 confirm_button_wh = {confirm_window_wh.x * 0.25f, wh.y * 0.05f};
            ImGui::SetCursorPos(
                {confirm_window_wh.x * 0.15f, confirm_window_wh.y * 0.65f});
            if (ImGui::Button("Yes", confirm_button_wh)) {
                level_number = 1;
                new_game();
                save_game();
                crow::audio::play_sfx(crow::audio::MENU_OK);
            }
            ImGui::SetCursorPos({confirm_window_wh.x * 0.6f, confirm_window_wh.y * 0.65f});
            if (ImGui::Button("No", confirm_button_wh)) {
                crow::audio::play_sfx(crow::audio::MENU_OK);
                menu_position = 0;
            }

            ImGui::SetWindowFocus();
            ImGui::End();
        }
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
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
      // set size parameters for the pause button icon
      ImVec2 pause_button_xy = {wh.x - (0.0333333f * wh.x), 0};
      ImVec2 pause_button_wh = {0.0333333f * wh.x, 0.0592592592592593f * wh.y};
      ImGui::SetNextWindowPos(pause_button_xy, ImGuiCond_Always);
      ImGui::SetNextWindowSize(pause_button_wh, ImGuiCond_Always);
        // color for bg of button
        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.f, 1.f, 1.f, 0.1f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.9f, 1.f, 0.85f, 0.2f});

        // finally create the pause button
        ImGui::Begin("Pause", nullptr, texture_flag);
        if (ImGui::ImageButton(textures[texture_list::GUI_PAUSE], pause_button_wh, { 0, 0 }, { 1, 1 }, -1, { 0, 0, 0, 0 }, { 1, 1, 1, 0.8f })) {
            crow::audio::play_sfx(crow::audio::MENU_OK);
            current_state = crow::game_manager::game_state::PAUSED;
        }

        ImGui::End();
        //// all texture-only GUI items should be before this line as it resets
        /// the / GUI window styling back to default
        ImGui::PopStyleVar(4);
        // pop the color too
        ImGui::PopStyleColor(3);
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
            current_state = game_state::SETTINGS;
            menu_position = 20;
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
        imgui_centertext(std::string("Quit to main menu?"), 4.0f, confirm_window_wh);
        ImGui::NewLine();
        imgui_centertext(std::string("Unsaved progress will be lost."), 4.0f, confirm_window_wh);

        ImVec2 confirm_button_wh = {confirm_window_wh.x * 0.25f, wh.y * 0.05f};
        ImGui::SetCursorPos(
            {confirm_window_wh.x * 0.15f, confirm_window_wh.y * 0.65f});
        if (ImGui::Button("Yes", confirm_button_wh)) {
          current_state = game_state::MAIN_MENU;
          crow::audio::play_sfx(crow::audio::MENU_OK);
          menu_position = 0;
          end_game();
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
          new_game();
          current_state = game_state::PLAYING;
          // TODO::selecting this option is the same as hitting the continue button
          // on the main menu
        }
        ImGui::SetCursorPos({wh.x * 0.6f, wh.y * 0.65f});
        if (ImGui::Button("Give up", game_over_button_wh)) {
          crow::audio::play_sfx(crow::audio::MENU_OK);
          current_state = game_state::MAIN_MENU;
          menu_position = 0;
        }
      }
      ImGui::End();
      ImGui::PopStyleVar(1);
    }

    void game_manager::draw_control_message(ImVec2 wh) {
        if (level_number > 1) return;
          ImVec2 info_window_xy = {wh.x * 0.74f, wh.y * 0.75f};
          ImVec2 info_window_wh = {wh.x * 0.26f, wh.y * 0.25f};
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
            std::string ptext = "Left click to move";
            float text_size = ImGui::GetFontSize() * ptext.size() / 2;
            ImGui::SameLine();
            ImGui::Text(ptext.c_str());
          }
          ImGui::NewLine();
          {
            std::string ptext = "Click on objects to interact";
            float text_size = ImGui::GetFontSize() * ptext.size() / 2;
            ImGui::SameLine();
            ImGui::Text(ptext.c_str());
          }
          ImGui::SetWindowFontScale(1.4f);
          ImGui::NewLine();
          ImGui::NewLine();
          {
            std::string ptext = "Right click on the rooms in";
            float text_size = ImGui::GetFontSize() * ptext.size() / 2;
            ImGui::SameLine();
            ImGui::Text(ptext.c_str());
          }
          ImGui::NewLine();
          {
            std::string ptext = "the minimap to switch views.";
            ImGui::SameLine();
            ImGui::Text(ptext.c_str());
          }

      ImGui::End();
    }

    void game_manager::draw_oxygen_remaining(ImVec2 wh) {
        if (!current_level.oxygen_console || !current_level.oxygen_console->is_broken) return;

        float scale = 2;
        
        std::string text = "OXYGEN REMAINING";
        float text_size =
            wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

        ImVec2 oxy_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.15f,
                                wh.y * 0.02f};
        ImVec2 oxy_window_wh = {text_size + wh.x * 0.3f,
                                wh.y * 0.048f + wh.y * 0.08f};

        // cgheck to see if the mouse is hovering over the window so that
        // we can render it more lightly if it is
        bool hovering = (mouse_pos_gui.x < static_cast<double>(oxy_window_xy.x) + static_cast<double>(oxy_window_wh.x) &&
          mouse_pos_gui.y < static_cast<double>(oxy_window_xy.y) + static_cast<double>(oxy_window_wh.y) &&
          mouse_pos_gui.x > static_cast<double>(oxy_window_xy.x) &&
            static_cast<double>(mouse_pos_gui.y) > static_cast<double>(oxy_window_xy.y));
        if (hovering) ImGui::SetNextWindowBgAlpha(0.15f);
        else ImGui::SetNextWindowBgAlpha(0.65f);
        
        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.4f, 0.65f, 0.99f, 0.5f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.65f, 0.99f, 0.5f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.4f, 0.65f, 0.99f, 0.5f});

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
        ImVec2 oxy_bar_wh = { (oxy_window_wh.x * 0.92f) * (current_level.selected_room->oxygen /
                current_level.selected_room->oxygen_max), oxy_window_wh.y * 0.7f};
        // positioning
        if (current_level.selected_room->oxygen) ImGui::SetCursorPos({oxy_window_wh.x * 0.04f, oxy_window_wh.y * 0.15f});  // positioning

        ImGui::Button("", oxy_bar_wh);
        ImGui::PopStyleColor(3);

        ImGui::End();
    }

    void game_manager::draw_pressure_remaining(ImVec2 wh) {
        if (!current_level.pressure_console || !current_level.pressure_console->is_broken) { return; }

        float scale = 2;
        
        std::string text = "PRESSURE LEVEL";
        float text_size =
            wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

        ImVec2 pressure_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.15f,
                                wh.y * 0.02f};
        ImVec2 pressure_window_wh = {text_size + wh.x * 0.3f,
                                wh.y * 0.048f + wh.y * 0.08f};

        // cgheck to see if the mouse is hovering over the window so that
        // we can render it more lightly if it is
        bool hovering = (mouse_pos_gui.x < static_cast<double>(pressure_window_xy.x) + static_cast<double>(pressure_window_wh.x) &&
          mouse_pos_gui.y < static_cast<double>(pressure_window_xy.y) + static_cast<double>(pressure_window_wh.y) &&
          mouse_pos_gui.x > static_cast<double>(pressure_window_xy.x) &&
            static_cast<double>(mouse_pos_gui.y) > static_cast<double>(pressure_window_xy.y));
        if (hovering) ImGui::SetNextWindowBgAlpha(0.15f);
        else ImGui::SetNextWindowBgAlpha(0.65f);
        
        ImGui::PushStyleColor(ImGuiCol_Button,        { 0.4f, 0.65f, 0.99f, 0.5f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.65f, 0.99f, 0.5f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.4f, 0.65f, 0.99f, 0.5f});

        ImGui::SetNextWindowPos(pressure_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(pressure_window_wh, ImGuiCond_Always);
        ImGui::Begin("pressure_wind_p", 0,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize);

        ImGui::SetWindowFontScale(wh.x / 960.f * scale);
        ImGui::SetCursorPos({wh.x * 0.15f, wh.y * 0.04f});
        ImGui::Text(text.c_str());

        // draw the bar using a button, just because we can
        // TODO::load a texture instead of something as hilarious as a BUTTON
        ImVec2 pressure_bar_wh = { (pressure_window_wh.x * 0.92f) * (current_level.pressure_console->pressure /
                current_level.pressure_console->pressure_max), pressure_window_wh.y * 0.7f};
        // positioning
        ImGui::SetCursorPos({pressure_window_wh.x * 0.04f, pressure_window_wh.y * 0.15f});  // positioning

        if (current_level.pressure_console->pressure) ImGui::Button("", pressure_bar_wh);

        ImGui::PopStyleColor(3);

        ImGui::End();
    }

    void game_manager::draw_options_menu(ImVec2 wh) {
        bool in_game = current_level.rooms.size();

        ImVec2 sm_window_xy = {0, 0};
        ImGui::SetNextWindowPos(sm_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        if (!in_game) ImGui::SetNextWindowBgAlpha(1);
        // menu drawing starts here
        ImGui::Begin("Options", 0,
                    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize);
        imgui_centertext(std::string("Options"), 4.0f, wh);
        ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine();
        ImGui::SetWindowFontScale(2);

        // draw sliders
        ImGui::NewLine();
        ImGui::SetCursorPosX(wh.x * 0.085f);
        ImGui::SliderFloat("   Master Volume", &audio::all_volume, 0.0f, 1.0f, " % .2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::NewLine();
        ImGui::SetCursorPosX(wh.x * 0.085f);
        ImGui::SliderFloat("    BGM Volume", &audio::bgm_volume, 0.0f, 1.0f, " % .2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::NewLine();
        ImGui::SetCursorPosX(wh.x * 0.085f);
        ImGui::SliderFloat("    SFX Volume", &audio::sfx_volume, 0.0f, 1.0f, " % .2f", ImGuiSliderFlags_AlwaysClamp);

        // set audio volume
        audio::update_volume();

        ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine();

        // fullscreen button
        BOOL fs;
        bool fs_b;
        p_impl->swapchain->GetFullscreenState(&fs, nullptr);
        fs_b = fs;
        ImGui::SetCursorPosX(wh.x * 0.435f);
        if (ImGui::Checkbox("Fullscreen", &fs_b)) {
            p_impl->swapchain->SetFullscreenState(fs_b, nullptr);
        }



        
        ImVec2 quit_button_wh = {wh.x * 0.2f, wh.y * 0.05f};
        ImGui::SetCursorPos({(wh.x - quit_button_wh.x) / 2, wh.y * 0.85f});
        if (ImGui::Button("Back", quit_button_wh)) {
            // if a level is currently loaded, we can safely assume we are in-game. return to pause menu
            if (in_game) {
                current_state = game_state::PAUSED;
            // return to main menu
            } else {
                current_state = game_state::MAIN_MENU;
            }
			save_game();
            crow::audio::play_sfx(crow::audio::MENU_OK);
            menu_position = 0;
        }



      ImGui::End();

      // no longer drawing borderless
      ImGui::PopStyleVar(1);
    }

}  // namespace crow
