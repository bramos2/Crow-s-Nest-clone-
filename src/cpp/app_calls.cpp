#include <liblava-extras/fbx.hpp>

#include <imgui.h>

#include "../hpp/audio.hpp"
#include "../hpp/cross.hpp"
#include "../hpp/game_manager.hpp"
#include "../hpp/pipeline.hpp"

namespace crow {

void game_manager::init_app() {
  // setting up config
  config.info.app_name = "Crow's Nest";
  config.cmd_line = {argc, argv};
  config.info.req_api_version = lava::api_version::v1_2;

  // creating the app
  app = new lava::app(config);
  app->manager.on_create_param = [](lava::device::create_param& param) {};
  app->setup();

  // setting up audio
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  // camera buffer
  crow::initialize_debug_camera(app->camera);
  camera_buffer_data = {
      app->camera.get_view_projection(),
  };
  camera_buffer.create_mapped(
      app->device, &camera_buffer_data, sizeof(camera_buffer_data),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  // world buffer
  world_matrix_buffer.create_mapped(app->device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  descriptor_pool = lava::make_descriptor_pool();
  descriptor_pool->create(app->device,
                          {
                              {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50},
                              {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 50},
                              {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 40},
                          },
                          180);

  app->on_create = on_create();

  app->on_destroy = on_destroy();

  app->on_update = on_update();

  app->imgui.on_draw = imgui_on_draw();

  app->input.mouse_button.listeners.add(mouse_events());

  app->add_run_end([&]() { crow::audio::cleanup(); });
}

auto game_manager::on_create() -> lava::app::create_func {
  return [&]() {
    // lava::render_pass::ptr render_pass = app->shading.get_pass();

    lava::VkVertexInputAttributeDescriptions vertex_attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal)},
    };

    std::vector<crow::shader_module> environment_shaders = {{
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/simple.vert.spv",
            .flags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/simple.frag.spv",
            .flags = VK_SHADER_STAGE_FRAGMENT_BIT,
        },
    }};

    // Global buffers:
    environment_descriptor_layouts[0] =
        crow::create_descriptor_layout(*app, crow::global_descriptor_bindings);
    // Render-pass buffers:
    environment_descriptor_layouts[1] =
        crow::create_descriptor_layout(*app, crow::simple_render_pass_bindings);
    // Material buffers:
    environment_descriptor_layouts[2] =
        crow::create_descriptor_layout(*app, crow::simple_material_bindings);
    // Object buffers:
    environment_descriptor_layouts[3] = crow::create_descriptor_layout(
        *app,
        {
            crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                     .binding_slot = 0,
                                     .descriptors_count = 1},
        });

    environment_descriptor_sets = crow::create_descriptor_sets(
        environment_descriptor_layouts, descriptor_pool);

    VkWriteDescriptorSet const write_ubo_global{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = camera_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_pass{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[1],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_material{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[2],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_object{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[3],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };

    app->device->vkUpdateDescriptorSets({write_ubo_global, write_ubo_pass,
                                         write_ubo_material, write_ubo_object});

    // Create pipelines.
    environment_pipeline = crow::create_rasterization_pipeline(
        *app, environment_pipeline_layout, environment_shaders,
        environment_descriptor_layouts, vertex_attributes);
    return true;
  };
}

auto game_manager::on_destroy() -> lava::app::destroy_func {
  return [&]() {
    for (size_t i = 0; i < 4; i++) {
      environment_descriptor_layouts[i]->destroy();
    }

    descriptor_pool->destroy();
    environment_pipeline->destroy();
    environment_pipeline_layout->destroy();
  };
}

auto game_manager::on_update() -> lava::app::update_func {
  return [&](lava::delta dt) {
    switch (current_state) {
      case crow::game_manager::game_state::MAIN_MENU: {
        break;
      }
      case crow::game_manager::game_state::PLAYING: {
        app->camera.update_view(dt, app->input.get_mouse_position());
        camera_buffer_data.projection_view = app->camera.get_view_projection();
        memcpy(camera_buffer.get_mapped_data(), &camera_buffer_data,
               sizeof(camera_buffer_data));

        crow::path_through(player_data, entities, crow::entity::WORKER, dt);

        for (size_t i = 0; i < entities.current_size; i++) {
          entities.update_transform_data(i, dt);
          entities.update_transform_buffer(i);
        }

        render_game();
        break;
      }
      case crow::game_manager::game_state::PAUSED: {
        render_game();
        break;
      }
      case crow::game_manager::game_state::SETTINGS: {
        break;
      }
      case crow::game_manager::game_state::CREDITS: {
        break;
      }
      case crow::game_manager::game_state::EXIT: {
        break;
      }
      default: {
        render_game();
        break;
      }
    }

    return true;
  };
}

// TODO: Clean up and reorganize how menus are drawn and selected
auto game_manager::imgui_on_draw() -> lava::imgui::draw_func {
  return [&] {
    switch (current_state) {
      case crow::game_manager::game_state::MAIN_MENU: {
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
                     ImGuiWindowFlags_NoDecoration |
                         ImGuiWindowFlags_NoCollapse |
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
        break;
      }
      case crow::game_manager::game_state::PLAYING: {
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
        ImVec2 pause_button_wh = {0.0333333f * wh.x,
                                  0.0592592592592593f * wh.y};
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

        // start of minimap processing
        minimap.draw_call(app);
        // end of minimap processing
        break;
      }
      case crow::game_manager::game_state::PAUSED: {
        // set size parameters for the pause window
        glm::vec2 wh = app->window.get_size();
        ImVec2 pause_window_xy = {wh.x * 0.35f, wh.y * 0.275f};
        ImVec2 pause_window_wh = {wh.x * 0.3f, wh.y * 0.45f};
        ImGui::SetNextWindowPos(pause_window_xy, ImGuiCond_Always);
        ImGui::SetNextWindowSize(pause_window_wh, ImGuiCond_Always);
        ImGui::Begin("Pause Window", 0,
                     ImGuiWindowFlags_NoDecoration |
                         ImGuiWindowFlags_NoCollapse |
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
        break;
      }
      case crow::game_manager::game_state::SETTINGS: {
        break;
      }
      case crow::game_manager::game_state::CREDITS: {
        break;
      }
      case crow::game_manager::game_state::EXIT: {
        break;
      }
      default: {
        break;
      }
    }
  };
}

auto game_manager::mouse_events() -> lava::mouse_button_event::func {
  return [&](lava::mouse_button_event::ref click) {
    if (click.released(lava::mouse_button::left)) {
      return l_click_update();
    }
    if (click.released(lava::mouse_button::right)) {
      return true;
    }
    return false;
  };
}

auto game_manager::run_end() -> lava::frame::run_end_func_ref {
  return [&]() { crow::audio::cleanup(); };
}

}  // namespace crow