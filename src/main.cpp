#include <liblava-extras/fbx.hpp>
#include <liblava/lava.hpp>

#include <imgui.h>

#include "hpp/pipeline.hpp"
#include "liblava/resource/mesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

#include "../debug_camera_control/debug_camera_control.hpp"
#include "hpp/component.hpp"
//#include "hpp/component_box_collision.hpp"
//#include "hpp/component_player.hpp"
//#include "hpp/component_sphere_collision.hpp"
#include "hpp/geometry.hpp"
#include "hpp/object.hpp"

auto main() -> int {
  lava::frame_config config;
  lava::app app(config);
  app.manager.on_create_param = [](lava::device::create_param& param) {};
  app.setup();

  app.camera.rotation_speed = 250;
  app.camera.movement_speed += 10;
  app.camera.position = lava::v3(0.0f, -4.036f, 8.304f);
  app.camera.rotation = lava::v3(-15, 0, 0);
  app.camera.set_movement_keys(debug_key_up, debug_key_down, debug_key_left,
                               debug_key_right);

  // Temporary geometry.
  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;
  world_matrix_buffer.create_mapped(app.device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mesh::ptr cube = lava::make_mesh();
  std::string fbx_path = ROOT_PATH;
  fbx_path.append("/ext/lava-fbx/ext/OpenFBX/runtime/a.FBX");  // Deer model
  ofbx::IScene* scene = lava::extras::load_fbx_scene(fbx_path.c_str());
  std::cout << "Loaded FBX scene.\n";

  lava::extras::fbx_data fbx_data = lava::extras::load_fbx_model(scene);
  cube->add_data(fbx_data.mesh_data);
  cube->create(app.device);

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
  VkDescriptorSet environment_descriptor_set = VK_NULL_HANDLE;
  crow::descriptor_writes_stack descriptor_writes;

  crow::entities entities;

  app.on_create = [&]() {
    lava::render_pass::ptr render_pass = app.shading.get_pass();

    lava::VkVertexInputAttributeDescriptions vertex_attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal)},
    };

    std::vector<crow::shader_module> environment_shaders = {{
        crow::shader_module{
            .file_name = "../res/simple.vert.spv",
            .flags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        crow::shader_module{
            .file_name = "../res/simple.frag.spv",
            .flags = VK_SHADER_STAGE_FRAGMENT_BIT,
        },
    }};

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

    // TODO(conscat): Push to stack.
    VkWriteDescriptorSet const write_ubo_global{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = environment_descriptor_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = app.camera.get_descriptor_info(),
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

    // Create entities.
    lava::mesh::ptr player_mesh = lava::make_mesh();
    lava::mesh_data player_mesh_data =
        lava::create_mesh_data(lava::mesh_type::cube);
    player_mesh->add_data(player_mesh_data);
    player_mesh->create(app.device);
    entities.meshes[crow::entity::WORKER] = player_mesh;
    entities.initialize_transforms(app, crow::entity::WORKER,
                                   &environment_descriptor_sets,
                                   &descriptor_writes);
    entities.velocities[crow::entity::WORKER] = glm::vec3{0.1f, 0, 0};
    crow::update_descriptor_writes(app, &descriptor_writes);

    // Create pipelines.
    environment_pipeline = crow::create_rasterization_pipeline(
        app, environment_pipeline_layout, environment_shaders,
        environment_descriptor_layouts, vertex_attributes);
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
          fmt::print("left mouse clicked ");
          return true;
        }
        return false;
      });

  app.imgui.on_draw = [&]() {
    // need this for having the GUI items scale with the window size
    glm::vec2 wh = app.window.get_size();
    // pass this flag into ImGui::Begin when you need to spawn a window that
    // only contains a texture
    const int texture_flag =
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

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
    ImGui::Begin("Pause", 0, texture_flag);
    if (ImGui::ImageButton(0 /* INSERT TEXTURE POINTER HERE */,
                           pause_button_wh)) {
      fmt::print("game paused! just kidding...\n");
      /* gamepaused GUI code can go here */
    }
    ImGui::End();

    // set size parameters for the item window
    ImVec2 item_window_xy = {wh.x * 0.025f, wh.y * 0.5f};
    ImVec2 item_window_wh = {wh.x * 0.0833333333f, wh.y * 0.148148148148f};
    ImGui::SetNextWindowPos(item_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(item_window_wh, ImGuiCond_Always);
    // finally create the window
    ImGui::Begin("Item", 0, texture_flag);
    ImGui::Image(0 /* INSERT TEXTURE POINTER HERE */, item_window_wh);
    ImGui::End();
    // all texture-only GUI items should be before this line as it resets the
    // GUI window styling back to default
    ImGui::PopStyleVar(3);

    // set size parameters for the minimap window
    ImVec2 minimap_window_xy = {wh.x * 0.025f, wh.y * 0.666f};
    ImVec2 minimap_window_wh = {wh.x * 0.4f, wh.y * 0.3f};
    ImGui::SetNextWindowPos(minimap_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(minimap_window_wh, ImGuiCond_Always);
    // finally create the window
    ImGui::Begin("Facility Map", 0,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);
    ImGui::Text("there is supposed to be a map here but it doesnt exist yet");
    ImGui::End();

    // debug window
#ifdef _DEBUG
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
      ImGui::End();
    }
#endif
  };

  app.on_update = [&](lava::delta dt) {
    if (app.camera.activated()) {
      app.camera.update_view(lava::to_dt(app.run_time.delta),
                             app.input.get_mouse_position());
    }

    // actual game loop; execute the entire game by simply cycling through
    // the array of objects
    for (size_t i = 0; i < entities.pcomponents.size(); i++) {
    }
    entities.update_transform_data(crow::entity::WORKER, dt);
    entities.update_transform_buffer(crow::entity::WORKER);

    environment_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4,
          environment_descriptor_sets.data(), 0, nullptr);
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

      for (auto& mesh : entities.meshes) {
        if (mesh) {  // TODO(conscat): Sort entities instead.
          mesh->bind_draw(cmd_buf);
        }
      }
    };

    return true;
  };

  app.add_run_end([&]() {
    cube->destroy();
    // for (auto& meshes : meshes) {
    //   if (meshes) {
    //     meshes->destroy();
    //   }
    // }
  });

  return app.run();
}
