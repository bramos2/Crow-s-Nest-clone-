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
            draw_move_pos(wh);
            draw_pause_button(wh);
            draw_control_message(wh);
            draw_oxygen_remaining(wh);
            draw_pressure_remaining(wh);
            draw_sd_timer(wh);
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
        case game_state::S_SPLASH_FS:
        case game_state::S_SPLASH_GD:
        case game_state::S_SPLASH_LV:
            draw_splash(wh);
            break;
        case game_state::CREDITS:
            draw_credits(wh);
            break;
        case game_state::GAME_WIN:
            draw_game_win_screen(wh);
            break;
        case game_state::LEVEL_WIN:
            draw_level_win_screen(wh);
            break;
        }

        if (debug_mode && current_state == game_state::PLAYING) {
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

        ImVec2 logo_wh = { wh.x * 0.5f, wh.y * 0.2f };
        ImVec2 logo_xy = { (wh.x - logo_wh.x) / 2.0f, (wh.y * 0.45f - logo_wh.y) / 2.0f };
        // draw game logo
        ImGui::SetCursorPos(logo_xy);
        ImGui::Image(textures[texture_list::GUI_LOGO], logo_wh);


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
                crow::audio::play_sfx(crow::audio::MENU_OK);
                current_state = game_state::CREDITS;
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

    void game_manager::draw_move_pos(ImVec2 wh) {
        // player not moving
        if (!player_data.path_result.size() || !current_level.selected_room->has_player) return;

        const int texture_flag =
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;

        float2e tpos = player_data.path_result[0];
        if (player_data.target != nullptr) tpos = current_level.selected_room->tiles.get_tile_wpos(player_data.target->x, player_data.target->y);

        float4_a tpos_4;
        tpos_4.x = tpos.x; tpos_4.y = 0; tpos_4.z = tpos.y; tpos_4.w = 1; 
        //tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose((DirectX::XMMATRIX&)view.view_mat));
        //tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)view.proj_final);
        tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose(view.view_final));
        tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose(view.proj_final));

        
        float2e tpos_ndc = {(((tpos_4.x / tpos_4.w) + 1) * (wh.x / 2)),
                            ((1 - (tpos_4.y / tpos_4.w)) * (wh.y / 2))};

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
        // set size parameters for the pause button icon
        ImVec2 select_wh = {0.0666666f * wh.x, 0.1185185185185186f * wh.y};
        ImVec2 select_xy = {tpos_ndc.x - select_wh.x / 2, tpos_ndc.y - select_wh.y / 2};
        ImGui::SetNextWindowPos(select_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(select_wh, ImGuiCond_Always);

        // finally create the select icon
        ImGui::Begin("w_move_to", nullptr, texture_flag);
        ImGui::Image(textures[texture_list::GUI_SELECT2], select_wh);

        ImGui::End();
        ImGui::PopStyleVar(4);
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

        // actual size of the window cuz imgui is weirdo
        ImVec2 real_size = get_window_size();
        
        // finding for possible interact thingy
		float3e mouse_point = crow::mouse_to_floor(view, mouse_pos, static_cast<int>(real_size.x), static_cast<int>(real_size.y));
        if (mouse_point.y == -1) return;

		const crow::tile* clicked_tile = current_level.selected_room->get_tile_at(float2e(mouse_point.x, mouse_point.z));

        if (!clicked_tile) return;

		crow::interactible* target = nullptr;

		for (auto& i : current_level.selected_room->objects) {
			if (clicked_tile->row != i->y || clicked_tile->col != i->x) {
				continue;
			}

			target = i;
			break;
		}

        if (!target) return;
        // get position of TARGET
        float2e tpos = current_level.selected_room->tiles.get_tile_wpos(clicked_tile->col, clicked_tile->row);
        float4_a tpos_4;
        tpos_4.x = tpos.x; tpos_4.y = 0; tpos_4.z = tpos.y; tpos_4.w = 1; 
        //tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose((DirectX::XMMATRIX&)view.view_mat));
        //tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)view.proj_final);
        tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose(view.view_final));
        tpos_4 = MatrixVectorMult(tpos_4, (float4x4_a&)DirectX::XMMatrixTranspose(view.proj_final));

        
        float2e tpos_ndc = {(((tpos_4.x / tpos_4.w) + 1) * (wh.x / 2)),
                            ((1 - (tpos_4.y / tpos_4.w)) * (wh.y / 2))};

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
        // set size parameters for the pause button icon
        ImVec2 select_wh = {0.0666666f * wh.x, 0.1185185185185186f * wh.y};
        ImVec2 select_xy = {tpos_ndc.x - select_wh.x / 2, tpos_ndc.y - select_wh.y / 2};
        ImGui::SetNextWindowPos(select_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(select_wh, ImGuiCond_Always);

        // finally create the select icon
        ImGui::Begin("Select", nullptr, texture_flag);
        ImGui::Image(textures[texture_list::GUI_SELECT], select_wh);

        ImGui::End();
        ImGui::PopStyleVar(4);
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

    void game_manager::draw_sd_timer(ImVec2 wh) {
        if (self_destruct_timer < 0) { return; }

        float scale = 5;
        

        std::string text;

        // load the time remaining into the string
        int time_int = (int)self_destruct_timer;
        int time_sec = time_int % 60;
        int time_min = time_int / 60;
        
        int time_sec_10 = time_sec / 10;
        int time_sec_01 = time_sec % 10;

        int time_ms = (int)((self_destruct_timer - time_int) * 1000);
        int time_ms_100 = time_ms / 100;
        int time_ms_010 = time_ms / 10 % 10;
        int time_ms_001 = time_ms % 10;

        text += '0' + time_min;
        text += ":";
        text += '0' + time_sec_10;
        text += '0' + time_sec_01;
        text += ".";
        text += '0' + time_ms_100;
        text += '0' + time_ms_010;
        text += '0' + time_ms_001;

        float text_size =
            wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

        ImVec2 sd_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.15f,
                                wh.y * 0.02f};
        ImVec2 sd_window_wh = {text_size + wh.x * 0.3f,
                                wh.y * 0.048f + wh.y * 0.08f};

       ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::SetNextWindowPos(sd_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(sd_window_wh, ImGuiCond_Always);
        ImGui::Begin("sd_timer", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::SetWindowFontScale(wh.x / 960.f * scale);
        ImGui::SetCursorPos({wh.x * 0.15f, wh.y * -0.01f});
        ImGui::Text(text.c_str());

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

    void game_manager::draw_splash(ImVec2 wh) {
        // calculated value of the splash screen's fading
        float fade_val = 1;
        if (state_time < 0.5f) fade_val = state_time / 0.5f;
        if (state_time > 3.5f) fade_val = (4.0f - state_time) / 0.5f;

        // cover the screen with black cuz ye
        ImVec2 sp_window_xy = { 0, 0 };
        ImGui::SetNextWindowPos(sp_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0, 0, 0, 1 });
        // main menu drawing starts here
        ImGui::Begin("Splash", 0,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize);
        
        ImVec2 splash_wh = { 0, 0 };
        int i = 0;

        // hardcoded image sizes cuz no need to implement texture size acquiring
        switch (current_state) {
            case game_state::S_SPLASH_FS:
            splash_wh = { 160, 100 };
            break;
            case game_state::S_SPLASH_GD:
            splash_wh = { 3300, 2176 };
            i = 1;
            break;
            case game_state::S_SPLASH_LV:
            splash_wh = { 1284 * 0.65f, 754 * 0.65f };
            i = 2;
            break;
        }

        // prevent the logo from being larger than the screen
        if (splash_wh.x > wh.x) {
            splash_wh.y *= (wh.x / splash_wh.x);
            splash_wh.x *= (wh.x / splash_wh.x);
        }

        if (splash_wh.y > wh.y) {
            splash_wh.x *= (wh.y / splash_wh.y);
            splash_wh.y *= (wh.y / splash_wh.y);
        }

        // automagically center the texture based on its size
        ImVec2 splash_xy = { (wh.x - splash_wh.x) / 2.0f, (wh.y - splash_wh.y) / 2.0f };

        ImGui::SetCursorPos(splash_xy);
        ImGui::Image(textures[texture_list::SPLASH_FS + i], splash_wh, { 0, 0 }, { 1, 1 }, {1, 1, 1, fade_val});


        ImGui::End();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
    }

    void game_manager::draw_credits(ImVec2 wh) {
        // cover the screen with black cuz ye
        ImVec2 c_window_xy = { 0, 0 };
        ImGui::SetNextWindowPos(c_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        // main menu drawing starts here
        ImGui::Begin("Credit", 0,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize);

        // debug timer
        //ImGui::Text("time: %f", state_time);
        
        
        float c_pos = wh.y;
        float dist = clampf(state_time, 0, 76.f/1.9f) * -60 * 1.9f;
        for (auto& c : credits) {
            switch (c.type) {
            case credit::credit_type::IMAGE: {
                dist += wh.y * 0.075f;

                // automagically center the texture based on its size
                ImVec2 img_xy = { (wh.x - (c.image_w * wh.x)) / 2.0f, c_pos + dist };

                ImGui::SetCursorPos(img_xy);

                ImGui::Image(textures[c.image_id], { wh.x * c.image_w, wh.y * c.image_h });
                // this doesn't work properly and i don't know why, i don't have time to debug it
                // i'll just use blanks as a band-aid for now.
                dist += wh.y * c.image_h * 0.8f;
                } break;
            case credit::credit_type::TITLE:
                dist += wh.y * 0.075f;

                ImGui::SetCursorPosY(c_pos + dist);
                ImGui::NewLine();
                imgui_centertext(c.text, 4.0f, wh);
                dist += wh.y * 0.05f;
                break;
            case credit::credit_type::TEXT:
                dist += wh.y * 0.075f;

                ImGui::SetCursorPosY(c_pos + dist);
                ImGui::NewLine();
                imgui_centertext(c.text, 2.0f, wh);
                break;
            case credit::credit_type::BLANK:
                dist += wh.y * c.image_h;
                break; // don't draw anything
            }
        }
        
        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void game_manager::draw_level_win_screen(ImVec2 wh) {
        // cover the screen with black cuz ye
        ImVec2 win_window_xy = { 0, 0 };
        ImGui::SetNextWindowPos(win_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::SetNextWindowBgAlpha(clampf(state_time, 0, 1.0f) * 0.65f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        // main menu drawing starts here
        ImGui::Begin("Win", 0,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize);

        if (state_time > 0.5f) {
            ImGui::SetCursorPosY(wh.y * 0.25f);
            ImGui::NewLine();
            imgui_centertext("The worker and you survived this floor.", 2.0f, wh);
        
            if (state_time > 2.5f) {
                ImGui::SetCursorPosY(wh.y * 0.5f);
                ImGui::NewLine();
                imgui_centertext("On to next floor...", 2.0f, wh);
            
                if (state_time > 4.5f) {
                    ImGui::SetCursorPosY(wh.y * 0.75f);
                    ImGui::NewLine();
                    imgui_centertext("Click anywhere to continue.", 1.0f, wh);
        
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

    void game_manager::draw_game_win_screen(ImVec2 wh) {
        // cover the screen with black cuz ye
        ImVec2 win_window_xy = { 0, 0 };
        ImGui::SetNextWindowPos(win_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(wh, ImGuiCond_Always);
        // black background so you can't tell that there is anything going on
        // behind it, borderless so you can't tell it's just an imgui window
        ImGui::SetNextWindowBgAlpha(clampf(state_time, 0, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        // main menu drawing starts here
        ImGui::Begin("Win", 0,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize);

        if (state_time > 0.5f) {
            ImGui::SetCursorPosY(wh.y * 0.3f);
            ImGui::NewLine();
            
            if (good_ending) imgui_centertext("The facility blew up with the creature still inside.", 2.0f, wh);
            else imgui_centertext("The worker has escaped.", 2.0f, wh);
        
            if (state_time > 2.5f) {
                ImGui::SetCursorPosY(wh.y * 0.6f);
                ImGui::NewLine();
                if (good_ending) imgui_centertext("It was never seen again.", 2.0f, wh);
                else imgui_centertext("He is safe from harm... for now.", 2.0f, wh);
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(1);
    }

}  // namespace crow
