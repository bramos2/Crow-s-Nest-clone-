#include <liblava-extras/fbx.hpp>
#include <liblava-extras/raytracing.hpp>
#include <liblava/lava.hpp>

#include <imgui.h>

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
#include "hpp/entities.hpp"
#include "hpp/game_state.hpp"
#include "hpp/geometry.hpp"
#include "hpp/map.hpp"
#include "hpp/minimap.hpp"
#include "hpp/raytracing.hpp"

auto main() -> int {
  // soloud sound initialization
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  lava::frame_config config;
  lava::app app(config);
  app.config.surface.formats = {
      VK_FORMAT_B8G8R8A8_SRGB,
      // VK_FORMAT_R8G8B8A8_SRGB
  };
  lava::device::ptr device = crow::create_logical_device(app.manager);
  app.device = device.get();
  app.manager.on_create_param = [](lava::device::create_param& param) {};
  app.setup();

  crow::initialize_debug_camera(app.camera);
  crow::camera_device_data camera_buffer_data = {
      app.camera.get_view_projection(),
  };
  lava::buffer camera_buffer;
  camera_buffer.create_mapped(
      app.device, &camera_buffer_data, sizeof(camera_buffer_data),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  crow::minimap minimap({0.0f, 0.65f}, {0.4f, 0.35f});
  lava::mesh::ptr current_room_mesh;

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

  lava::mesh::ptr cube = lava::make_mesh();
  std::string fbx_path =
      crow::get_exe_path() + "../../res/fbx/deer.fbx";  // Deer model
  ofbx::IScene* scene = lava::extras::load_fbx_scene(fbx_path.c_str());
  std::cout << "Loaded FBX scene.\n";

  lava::extras::fbx_data fbx_data = lava::extras::load_fbx_model(scene);
  cube->add_data(fbx_data.mesh_data);
  cube->create(app.device);

  lava::queue::ref queue = app.device->graphics_queue();
  const size_t uniform_stride =
      lava::align_up(sizeof(crow::raytracing_uniform_data),
                     app.device->get_physical_device()
                         ->get_properties()
                         .limits.minUniformBufferOffsetAlignment);

  lava::pipeline_layout::ptr blit_pipeline_layout;
  lava::graphics_pipeline::ptr blit_pipeline;
  crow::descriptor_layouts shared_descriptor_layouts;
  VkDescriptorSet shared_descriptor_set = VK_NULL_HANDLE;

  lava::pipeline_layout::ptr raytracing_pipeline_layout;
  lava::extras::raytracing::raytracing_pipeline::ptr raytracing_pipeline;

  lava::extras::raytracing::shader_binding_table::ptr shader_binding;

  lava::descriptor::ptr raytracing_descriptor_set_layout;
  VkDescriptorSet raytracing_descriptor_set = VK_NULL_HANDLE;

  lava::extras::raytracing::top_level_acceleration_structure::ptr top_as;
  lava::extras::raytracing::bottom_level_acceleration_structure::list
      bottom_as_list;

  lava::buffer::ptr scratch_buffer;
  VkDeviceAddress scratch_buffer_address = 0;

  VkCommandPool command_pool = VK_NULL_HANDLE;
  auto descriptor_pool = crow::create_descriptor_pool(app);
  crow::descriptor_writes_stack descriptor_writes;
  crow::entities entities;

  lava::buffer::ptr instance_buffer;
  lava::buffer::ptr vertex_buffer;
  lava::buffer::ptr index_buffer;
  lava::buffer::ptr uniform_buffer;
  crow::raytracing_uniform_data uniform_data{};
  lava::image::ptr output_image;

  auto swapchain_callback =
      crow::create_swapchain_callback(app, shared_descriptor_set, uniform_data,
                                      output_image, command_pool, queue);
  app.target->add_callback(&swapchain_callback);

  // // room buffer creation
  // crow::descriptor_sets room_descriptor_sets;
  // crow::descriptor_layouts room_descriptor_layouts;

  // // setting up the gamestate
  // crow::game_state game_state;
  // game_state.current_state = game_state.PLAYING;
  // // points to important game data
  // game_state.environment_descriptor_sets = &shared_descriptor_sets;
  // game_state.descriptor_writes = &descriptor_writes;
  // game_state.minimap = &minimap;
  // game_state.entities = &entities;
  // game_state.app = &app;

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

    // // Global buffers:
    // room_descriptor_layouts[0] =
    //     crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // // Render-pass buffers:
    // room_descriptor_layouts[1] =
    //     crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // // Material buffers:
    // room_descriptor_layouts[2] =
    //     crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // // Object buffers:
    // room_descriptor_layouts[3] = crow::create_descriptor_layout(
    //     app,
    //     {
    //         crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //                                  .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
    //                                  .binding_slot = 0,
    //                                  .descriptors_count = 1},
    //     });

    // // Global buffers:
    // shared_descriptor_layouts[0] =
    //     crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // // Render-pass buffers:
    // shared_descriptor_layouts[1] =
    //     crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // // Material buffers:
    // shared_descriptor_layouts[2] =
    //     crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // // Object buffers:
    // shared_descriptor_layouts[3] = crow::create_descriptor_layout(
    //     app,
    //     {
    //         crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //                                  .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
    //                                  .binding_slot = 0,
    //                                  .descriptors_count = 1},
    //     });

    // shared_descriptor_sets = crow::create_descriptor_sets(
    //     shared_descriptor_layouts, descriptor_pool);

    // room_descriptor_sets =
    //     crow::create_descriptor_sets(room_descriptor_layouts, descriptor_pool);

    // // TODO(conscat): Push to stack.
    // VkWriteDescriptorSet const write_ubo_global{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = shared_descriptor_sets[0],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = camera_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_pass{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = shared_descriptor_sets[1],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_material{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = shared_descriptor_sets[2],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_object{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = shared_descriptor_sets[3],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    // };
    // app.device->vkUpdateDescriptorSets({write_ubo_global, write_ubo_pass,
    //                                     write_ubo_material, write_ubo_object});

    // VkWriteDescriptorSet const write_ubo_global_room{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = room_descriptor_sets[0],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = camera_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_pass_room{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = room_descriptor_sets[1],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_material_room{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = room_descriptor_sets[2],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    // };
    // VkWriteDescriptorSet const write_ubo_object_room{
    //     .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //     .dstSet = room_descriptor_sets[3],
    //     .dstBinding = 0,
    //     .descriptorCount = 1,
    //     .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     .pBufferInfo = room_matrix_buffer.get_descriptor_info(),
    // };

    // app.device->vkUpdateDescriptorSets(
    //     {write_ubo_global_room, write_ubo_pass_room, write_ubo_material_room,
    //      write_ubo_object_room});

    // Create pipelines.
    blit_pipeline = crow::create_rasterization_pipeline(
        app, blit_pipeline_layout, environment_shaders,
        shared_descriptor_layouts, vertex_attributes);

    // Create entities.
    lava::mesh::ptr player_mesh = lava::make_mesh();
    lava::mesh_data player_mesh_data =
        lava::create_mesh_data(lava::mesh_type::cube);
    player_mesh->add_data(player_mesh_data);
    player_mesh->create(app.device);
    entities.meshes[crow::entity::WORKER] = player_mesh;
    // crow::new_game(game_state);
    // game_state.current_state = game_state.MAIN_MENU;

    return true;
  };

  app.on_destroy = [&]() {
    // TODO(conscat): Free all arrays of lava objects.
    shared_descriptor_layouts[0]->destroy();
    shared_descriptor_layouts[3]->destroy();
    descriptor_pool->destroy();
    blit_pipeline->destroy();
    blit_pipeline_layout->destroy();
  };

  app.input.mouse_button.listeners.add(
      [&](lava::mouse_button_event::ref click) {
        if (click.released(lava::mouse_button::left)) {
          fmt::print("left mouse clicked ");
          crow::audio::play_sfx(0);
          return true;
        }
        return false;
      });

  app.imgui.on_draw = [&]() {
    minimap.camera = &app.camera;
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
      // game_state.current_state = game_state.PAUSED;
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
    // crow::draw_menus(game_state, {wh.x, wh.y});

    // start of minimap processing
    minimap.draw_call(&app, current_room_mesh);
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
    // if (game_state.current_state == game_state.PLAYING) {
      app.camera.update_view(dt, app.input.get_mouse_position());
      camera_buffer_data.projection_view = app.camera.get_view_projection();
      memcpy(camera_buffer.get_mapped_data(), &camera_buffer_data,
             sizeof(camera_buffer_data));
    // }

    // actual game loop; execute the entire game by simply cycling through
    // the array of objects
    for (size_t i = 0; i < entities.pcomponents.size(); i++) {
    }
    entities.update_transform_data(crow::entity::WORKER, dt);
    entities.update_transform_buffer(crow::entity::WORKER);

    blit_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      // app.device->call().vkCmdBindDescriptorSets(
      //     cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, blit_pipeline_layout->get(),
      //     0, 4, room_descriptor_sets.data(), 0, nullptr);
      // if (current_room_mesh) {
      //   current_room_mesh->bind_draw(cmd_buf);
      // }
      // app.device->call().vkCmdBindDescriptorSets(
      //     cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, blit_pipeline_layout->get(),
      //     0, 4, shared_descriptor_sets.data(), 0, nullptr);
      // // TODO(conscat): Write a bind_descriptor_sets() method.
      // // environment_pipeline_layout->bind_descriptor_set(
      // //     cmd_buf, environment_descriptor_sets[0], 0);
      // // environment_pipeline_layout->bind_descriptor_set(
      // //     cmd_buf, environment_descriptor_sets[1], 1);
      // // environment_pipeline_layout->bind_descriptor_set(
      // //     cmd_buf, environment_descriptor_sets[2], 2);
      // // environment_pipeline_layout->bind_descriptor_set(
      // //     cmd_buf, environment_descriptor_sets[3], 3);
      // // cube->bind_draw(cmd_buf);

      // for (auto& mesh : entities.meshes) {
      //   if (mesh) {  // TODO(conscat): Sort entities instead.
      //     mesh->bind_draw(cmd_buf);
      //   }
      // }
    };

    // temp_position = crow::get_floor_point(app.camera);
    return true;
  };

  app.add_run_end([&]() {
    cube->destroy();
    crow::audio::cleanup();
    // crow::end_game(game_state);
    // for (auto& meshes : meshes) {
    //   if (meshes) {
    //     meshes->destroy();
    //   }
    // }
  });

  return app.run();
}
