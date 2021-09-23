#include <liblava-extras/fbx.hpp>
#include <liblava-extras/raytracing.hpp>
#include <liblava/lava.hpp>

#include <imgui.h>
#include <iostream>

#include "hpp/camera.hpp"
#include "hpp/device.hpp"
#include "hpp/pipeline.hpp"
#include "liblava/resource/mesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

#include "hpp/audio.hpp"
#include "hpp/component.hpp"
#include "hpp/cross.hpp"
#include "hpp/game_state.hpp"
#include "hpp/geometry.hpp"

auto main(int argc, char* argv[]) -> int {
  // soloud sound initialization
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  crow::game_state game_state;

  lava::frame_config config;
  config.info.app_name = "Crow's Nest";
  config.cmd_line = {argc, argv};
  config.info.req_api_version = lava::api_version::v1_2;
  lava::app app(config);
  app.manager.on_create_param = [](lava::device::create_param& param) {};
  app.setup();

  crow::initialize_debug_camera(app.camera);

  // TODO: move to game state (crashes when doing so at the moment, may not be
  // necessary)
  game_state.camera_buffer.create_mapped(
      app.device, &game_state.camera_buffer_data,
      sizeof(game_state.camera_buffer_data),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  // MESHES FOR BUILD PRESENTATION
  lava::mesh_data wall_cube_data =
      lava::create_mesh_data(lava::mesh_type::cube);

  lava::mesh::ptr wall1 = lava::make_mesh();  // center wall vertical
  wall_cube_data.scale_vector({12.0f, 4.0f, 1.0f});
  wall1->add_data(wall_cube_data);
  wall1->create(app.device);

  lava::mesh::ptr wall2 = lava::make_mesh();  // center wall horizontal
  wall_cube_data = lava::create_mesh_data(lava::mesh_type::cube);
  wall_cube_data.scale_vector({1.0f, 4.0f, 12.0f});
  wall2->add_data(wall_cube_data);
  wall2->create(app.device);

  wall_cube_data = lava::create_mesh_data(lava::mesh_type::cube);
  wall_cube_data.scale_vector({1.0f, 4.0f, 6.0f});

  lava::mesh::ptr wall3 = lava::make_mesh();  // north wall
  wall_cube_data.move({0, 0, 32});
  wall3->add_data(wall_cube_data);
  wall3->create(app.device);

  lava::mesh::ptr wall4 = lava::make_mesh();  // south wall
  wall_cube_data.move({0, 0, -64});
  wall4->add_data(wall_cube_data);
  wall4->create(app.device);

  wall_cube_data = lava::create_mesh_data(lava::mesh_type::cube);
  wall_cube_data.scale_vector({6.0f, 4.0f, 1.0f});

  lava::mesh::ptr wall5 = lava::make_mesh();  // west wall
  wall_cube_data.move({-32, 0, 0});
  wall5->add_data(wall_cube_data);
  wall5->create(app.device);

  lava::mesh::ptr wall6 = lava::make_mesh();  // east wall
  wall_cube_data.move({64, 0, 0});
  wall6->add_data(wall_cube_data);
  wall6->create(app.device);
  // *********** end meshes for build ***********//

  // Temporary geometry.
  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;
  world_matrix_buffer.create_mapped(app.device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mat4 room_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer room_matrix_buffer;
  room_matrix_buffer.create_mapped(app.device, &room_matrix_buffer_data,
                                   sizeof(room_matrix_buffer_data),
                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mat4 s_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer s_matrix_buffer;
  s_matrix_buffer.create_mapped(app.device, &s_matrix_buffer_data,
                                sizeof(s_matrix_buffer_data),
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  // lava::mesh::ptr cube = lava::make_mesh();

  lava::descriptor::pool::ptr descriptor_pool = lava::make_descriptor_pool();
  descriptor_pool->create(app.device,
                          {
                              {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50},
                              {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 50},
                              {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 40},
                          },
                          90);

  lava::graphics_pipeline::ptr environment_pipeline;
  lava::pipeline_layout::ptr environment_pipeline_layout;
  crow::descriptor_layouts environment_descriptor_layouts;
  // TODO(conscat): Streamline descriptor sets.
  crow::descriptor_sets environment_descriptor_sets;
  // VkDescriptorSet environment_descriptor_set = VK_NULL_HANDLE;
  crow::descriptor_writes_stack descriptor_writes;

  // crow::entities entities;

  // room buffer creation
  crow::descriptor_sets room_descriptor_sets;
  // crow::descriptor_writes_stack room_descriptor_writes;
  crow::descriptor_layouts room_descriptor_layouts;

  // sphynx buffer creation
  crow::descriptor_sets s_desc_sets;
  // crow::descriptor_writes_stack s_desc_writes;
  crow::descriptor_layouts s_desc_layouts;

  // setting up the gamestate
  game_state.desc_sets_list.resize(game_state.entities.transforms_data.size());
  game_state.desc_sets_list[crow::entity::WORKER] =
      &environment_descriptor_sets;  // player enviroment set
  game_state.desc_sets_list[crow::entity::SPHYNX] =
      &s_desc_sets;  // enemy ai enviroment set
  game_state.descriptor_writes = &descriptor_writes;
  game_state.app = &app;

  app.on_create = [&]() {
    lava::render_pass::ptr render_pass = app.shading.get_pass();

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
    room_descriptor_layouts[0] =
        crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // Render-pass buffers:
    room_descriptor_layouts[1] =
        crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // Material buffers:
    room_descriptor_layouts[2] =
        crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // Object buffers:
    room_descriptor_layouts[3] = crow::create_descriptor_layout(
        app,
        {
            crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                     .binding_slot = 0,
                                     .descriptors_count = 1},
        });

    // Global buffers:
    s_desc_layouts[0] =
        crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // Render-pass buffers:
    s_desc_layouts[1] =
        crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // Material buffers:
    s_desc_layouts[2] =
        crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // Object buffers:
    s_desc_layouts[3] = crow::create_descriptor_layout(
        app,
        {
            crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                     .binding_slot = 0,
                                     .descriptors_count = 1},
        });

    // Global buffers:
    environment_descriptor_layouts[0] =
        crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // Render-pass buffers:
    environment_descriptor_layouts[1] =
        crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // Material buffers:
    environment_descriptor_layouts[2] =
        crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // Object buffers:
    environment_descriptor_layouts[3] = crow::create_descriptor_layout(
        app,
        {
            crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                     .binding_slot = 0,
                                     .descriptors_count = 1},
        });

    environment_descriptor_sets = crow::create_descriptor_sets(
        environment_descriptor_layouts, descriptor_pool);

    room_descriptor_sets =
        crow::create_descriptor_sets(room_descriptor_layouts, descriptor_pool);

    s_desc_sets = crow::create_descriptor_sets(s_desc_layouts, descriptor_pool);

    // TODO(conscat): Push to stack.
    VkWriteDescriptorSet const write_ubo_global{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = game_state.camera_buffer.get_descriptor_info(),
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
    app.device->vkUpdateDescriptorSets({write_ubo_global, write_ubo_pass,
                                        write_ubo_material, write_ubo_object});

    VkWriteDescriptorSet const write_ubo_global_s{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = s_desc_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = game_state.camera_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_pass_s{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = s_desc_sets[1],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = s_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_material_s{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = s_desc_sets[2],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = s_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_object_s{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = s_desc_sets[3],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = s_matrix_buffer.get_descriptor_info(),
    };
    app.device->vkUpdateDescriptorSets({write_ubo_global_s, write_ubo_pass_s,
                                        write_ubo_material_s,
                                        write_ubo_object_s});

    VkWriteDescriptorSet const write_ubo_global_room{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = room_descriptor_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = game_state.camera_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_pass_room{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = room_descriptor_sets[1],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_material_room{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = room_descriptor_sets[2],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_object_room{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = room_descriptor_sets[3],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    };

    app.device->vkUpdateDescriptorSets(
        {write_ubo_global_room, write_ubo_pass_room, write_ubo_material_room,
         write_ubo_object_room});

    // Create pipelines.
    environment_pipeline = crow::create_rasterization_pipeline(
        app, environment_pipeline_layout, environment_shaders,
        environment_descriptor_layouts, vertex_attributes);

    crow::new_game(game_state);
    return true;
  };

  app.on_destroy = [&]() {
    // TODO(conscat): Free all arrays of lava objects.
    environment_descriptor_layouts[0]->destroy();
    environment_descriptor_layouts[3]->destroy();
    descriptor_pool->destroy();
    environment_pipeline->destroy();
    environment_pipeline_layout->destroy();
  };

  app.input.mouse_button.listeners.add(
      [&](lava::mouse_button_event::ref click) {
        if (click.released(lava::mouse_button::left)) {
          return left_click_update(game_state);
        }
        return false;
      });

  app.imgui.on_draw = [&]() {
    game_state.minimap.camera = &app.camera;
    // need this for having the GUI items scale with the window size
    glm::vec2 wh = app.window.get_size();
    // pass this flag into ImGui::Begin when you need to spawn a window that
    // only contains a texture
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
      if (game_state.current_state == game_state.PLAYING) {
        game_state.current_state = game_state.PAUSED;
      } else {
        game_state.current_state = game_state.PLAYING;
      }
    }

    ImGui::End();

    // set size parameters for the item window
    ImVec2 item_window_xy = {wh.x * 0.025f, wh.y * 0.5f};
    ImVec2 item_window_wh = {wh.x * 0.0833333333f, wh.y * 0.148148148148f};
    ImGui::SetNextWindowPos(item_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(item_window_wh, ImGuiCond_Always);
    // finally create the window
    ImGui::Begin("Item", nullptr, texture_flag);
    ImGui::Image(nullptr /* INSERT TEXTURE POINTER HERE */, item_window_wh);
    ImGui::End();
    // all texture-only GUI items should be before this line as it resets the
    // GUI window styling back to default
    ImGui::PopStyleVar(3);

    // render ALL the menus
    crow::draw_menus(game_state, {wh.x, wh.y});

    // start of minimap processing
    game_state.minimap.draw_call(
        &app, game_state.minimap.active_room
                  ->room_mesh);  // function parameter could be changed perhaps
                                 // end of minimap processing

    // debug window
#ifdef DEBUG_IMGUI
    // change to if (false) if the debug window keeps slapping you in the face
    // and that bothers you
    if (true) {
      // exists just for the fmt::print example (delete when we are actually
      // using the debug window)
      int example_integer = 25;
      ImVec2 debug_window_xy = {(wh.x * 0.05f), (wh.y * 0.05f)};
      ImVec2 debug_window_wh = {(wh.x * 0.4f), (wh.y * 0.3f)};
      ImGui::SetNextWindowPos(debug_window_xy, ImGuiCond_Once);
      ImGui::SetNextWindowSize(debug_window_wh, ImGuiCond_Always);
      ImGui::Begin("debug", 0, ImGuiWindowFlags_NoResize);
      ImGui::Text("dynamically loaded debug field here");
      ImGui::Text("dynamically loaded debug field here");
      ImGui::Text("dynamically loaded debug field here");
      ImGui::Text("yes it does fmt::print %f %f %i", debug_window_xy.x,
                  debug_window_xy.y, example_integer);
      ImGui::Spacing();
      ImGui::DragFloat3("position##camera", (lava::r32*)&app.camera.position,
                        0.01f);
      ImGui::DragFloat3("rotation##camera", (lava::r32*)&app.camera.rotation,
                        0.1f);
      ImGui::DragFloat3("position##mouse-point", (lava::r32*)&temp_position,
                        0.1f);
      ImGui::Spacing();
      ImGui::End();
    }
#endif
  };  // end imguiondraw

  app.on_update = [&](lava::delta dt) {
    // game logic is processed here
    crow::update(game_state, dt);

    // actual game loop; execute the entire game by simply cycling through
    // the array of objects

    // the rest of this update function is concern of the rendering
    environment_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4, room_descriptor_sets.data(),
          0, nullptr);
      if (game_state.minimap.active_room->room_mesh) {
        game_state.minimap.active_room->room_mesh->bind_draw(cmd_buf);
      }
      wall1->bind_draw(cmd_buf);
      wall2->bind_draw(cmd_buf);
      wall3->bind_draw(cmd_buf);
      wall4->bind_draw(cmd_buf);
      wall5->bind_draw(cmd_buf);
      wall6->bind_draw(cmd_buf);

      // this is annoying right now
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4,
          environment_descriptor_sets.data(), 0, nullptr);
      game_state.entities.meshes[crow::entity::WORKER]->bind_draw(cmd_buf);

      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4, s_desc_sets.data(), 0,
          nullptr);
      game_state.entities.meshes[crow::entity::SPHYNX]->bind_draw(cmd_buf);

      // TODO(conscat): Write a bind_descriptor_sets() method.
      // environment_pipeline_layout->bind_descriptor_set(
      //     cmd_buf, environment_descriptor_sets[0], 0);
      // environment_pipeline_layout->bind_descriptor_set(
      //     cmd_buf, environment_descriptor_sets[1], 1);
      // environment_pipeline_layout->bind_descriptor_set(
      //     cmd_buf, environment_descriptor_sets[2], 2);
      // environment_pipeline_layout->bind_descriptor_set(
      //     cmd_buf, environment_descriptor_sets[3], 3);
      // cube->bind_draw(cmd_buf);

      // for (auto& mesh : entities.meshes) {
      //  if (mesh) {  // TODO(conscat): Sort entities instead.
      //    mesh->bind_draw(cmd_buf);
      //  }
      //}
    };

    return true;
  };

  app.add_run_end([&]() {
    // cube->destroy();
    crow::audio::cleanup();
    crow::end_game(game_state);
    // for (auto& meshes : meshes) {
    //   if (meshes) {
    //     meshes->destroy();
    //   }
    // }
  });

  return app.run();
}
