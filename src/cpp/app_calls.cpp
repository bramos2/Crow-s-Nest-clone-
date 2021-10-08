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

        crow::path_through(player_data, entities, static_cast<size_t>(crow::entity::WORKER), dt);

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
        draw_main_menu();
        break;
      }
      case crow::game_manager::game_state::PLAYING: {
        draw_pause_button();
        minimap.draw_call(app);
        break;
      }
      case crow::game_manager::game_state::PAUSED: {
        draw_pause_menu();
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
      return r_click_update();
    }
    return false;
  };
}

auto game_manager::run_end() -> lava::frame::run_end_func_ref {
  return [&]() { crow::audio::cleanup(); };
}

}  // namespace crow