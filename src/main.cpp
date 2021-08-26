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

#include "hpp/component.hpp"
#include "hpp/geometry.hpp"
#include "hpp/map.hpp"
#include "hpp/minimap.hpp"
#include "hpp/object.hpp"
#include "hpp/audio.hpp"

// http://www.cplusplus.com/forum/general/11104/
#ifdef WIN32
#include <windows.h>
auto get_exe_path() -> std::string {
  std::array<char, MAX_PATH> result{};
  std::string full_path = std::string(
      result.data(), GetModuleFileName(NULL, result.data(), MAX_PATH));
  return std::string(full_path.substr(0, full_path.find_last_of('\\\\'))) + "/";
}
#else
#include <unistd.h>
auto get_exe_path() -> std::string {
  std::array<char, PATH_MAX - NAME_MAX> result{};
  ssize_t count =
      readlink("/proc/self/exe", result.data(), PATH_MAX - NAME_MAX);
  std::string full_path = std::string(result.data());
  return std::string(full_path.substr(0, full_path.find_last_of('/'))) + "/";
}
#endif

glm::vec3 temp_position = glm::vec3{0, 0, 0};

auto main() -> int {
  // soloud sound initialization
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  lava::frame_config config;
  lava::app app(config);
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
  crow::item_window item_w;
  item_w.screen_minr = {0.025f, 0.5f};
  item_w.screen_maxr = {0.0833333333f, 0.148148148148f};

  // Temporary geometry.
  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;
  world_matrix_buffer.create_mapped(app.device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mesh::ptr cube = lava::make_mesh();
  std::string fbx_path =
      get_exe_path() +
      "../../ext/lava-fbx/ext/OpenFBX/runtime/a.FBX";  // Deer model
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

  //-----map  generation testing----
  world_map temp_map_var(5, 5, 6);
  temp_map_var.set_block_size(200, 150, 20);
  temp_map_var.set_block_space(4, 3, 5);

  // minimap logic
  minimap.map_minc = {-300, -300};
  minimap.map_maxc = {300, 300};
  minimap.screen_minr = {0.0f, 0.65f};
  minimap.screen_maxr = {0.4f, 0.35f};
  minimap.resolution = {1920, 1080};
  minimap.set_window_size(app.window.get_size());
  temp_map_var.generate_map_blockout(
      {minimap.window_ext.x / 2, minimap.window_ext.y / 2});
  {
    /*glm::vec2 temp1;
    glm::vec2 temp2;
    temp_map_var.get_dimensions(temp1, temp2);
    int holdup = 0;
    minimap.mpos = {-temp2.x, -temp2.y, 0, 0};*/
  }
  temp_map_var.generate_block_rooms(4, 8);

  minimap.populate_map_data(temp_map_var);

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
            .file_name = get_exe_path() + "../../res/simple.vert.spv",
            .flags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        crow::shader_module{
            .file_name = get_exe_path() + "../../res/simple.frag.spv",
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
          crow::audio::play_sfx(0);
          return true;
        }
        return false;
      });

  app.imgui.on_draw = [&]() {
    minimap.app = &app;
    item_w.app = &app;

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

    minimap.draw_minimap();
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
    app.camera.update_view(dt, app.input.get_mouse_position());
    camera_buffer_data.projection_view = app.camera.get_view_projection();
    memcpy(camera_buffer.get_mapped_data(), &camera_buffer_data,
           sizeof(camera_buffer_data));

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

    temp_position = crow::get_floor_point(app.camera);
    return true;
  };

  app.add_run_end([&]() {
    cube->destroy();
  crow::audio::cleanup();
    // for (auto& meshes : meshes) {
    //   if (meshes) {
    //     meshes->destroy();
    //   }
    // }
  });

  return app.run();
}
