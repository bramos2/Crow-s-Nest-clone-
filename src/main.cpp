#include <liblava-extras/fbx.hpp>
#include <liblava-extras/raytracing.hpp>
#include <liblava/lava.hpp>

#include <imgui.h>

#include "hpp/camera.hpp"
#include "hpp/device.hpp"
#include "hpp/pipeline.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

#include "hpp/component.hpp"
#include "hpp/cross.hpp"
#include "hpp/geometry.hpp"
#include "hpp/minimap.hpp"
#include "hpp/object.hpp"

glm::vec3 temp_position = glm::vec3{0, 0, 0};

auto main(int argc, char* argv[]) -> int {
  lava::frame_config config;
  config.info.app_name = "Crow's Nest";
  config.cmd_line = {argc, argv};
  config.info.req_api_version = lava::api_version::v1_2;
  lava::app app(config);
  lava::device::ptr logical_device = crow::create_logical_device(app.manager);
  app.manager.on_create_param = [](lava::device::create_param& param) {};
  app.setup();
  crow::initialize_debug_camera(app.camera);
  // std::vector<crow::instance_data> instances;

  // The command buffer used for vkCmdBuildAccelerationStructureKHR and
  // vkCmdTraceRaysKHR must support compute. liblava's default queue has
  // graphics, compute, and transfer support, and the Vulkan spec guarantees
  // that this combination exists if the device supports graphics queues.
  lava::queue::ref graphics_queue = app.device->graphics_queue();
  crow::raytracing_uniform_data raytracing_uniform_data{};
  const size_t raytracing_uniform_stride =
      lava::align_up(sizeof(raytracing_uniform_data),
                     app.device->get_physical_device()
                         ->get_properties()
                         .limits.minUniformBufferOffsetAlignment);

  // full minimap setup
  crow::minimap minimap;
  crow::initialize_minimap(minimap);
  // minimap.mpos should be calculated, not hardcoded, and based on the rooms
  // that are added to the map by the map generation function.
  minimap.mpos = {-200, -200, 500, 200};
  // the rooms that currently are being added to the map are for testing
  // purposes only.
  minimap.rooms.push_back({220, 120, 100, 20});
  minimap.rooms.push_back({50, 20, 200, 100});
  minimap.rooms.push_back({330, 150, 35, 80});
  minimap.rooms.push_back({2, 2, 44, 300});
  minimap.rooms.push_back({550, 250, 200, 70});
  minimap.rooms.push_back({20, 20, 20, 10});
  // end minimap setup

  // Temporary geometry.
  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;
  world_matrix_buffer.create_mapped(app.device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mesh::ptr cube = lava::make_mesh();
  std::string fbx_path =
      crow::get_exe_path() +
      "../../ext/lava-fbx/ext/OpenFBX/runtime/a.FBX";  // Deer model
  ofbx::IScene* scene = lava::extras::load_fbx_scene(fbx_path.c_str());
  std::cout << "Loaded FBX scene.\n";

  lava::extras::fbx_data fbx_data = lava::extras::load_fbx_model(scene);
  cube->add_data(fbx_data.mesh_data);
  cube->create(app.device);

  VkCommandPool command_pool = VK_NULL_HANDLE;
  lava::descriptor::pool::ptr descriptor_pool = lava::make_descriptor_pool();
  descriptor_pool->create(
      app.device,
      {
          {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
          {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
          // TODO(conscat): More efficient sizes.
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50},
          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 50},
          {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 40},
      },
      90);

  lava::extras::raytracing::raytracing_pipeline::ptr raytracing_pipeline;
  lava::pipeline_layout::ptr raytracing_pipeline_layout;
  lava::pipeline_layout::ptr blit_pipeline_layout;
  lava::graphics_pipeline::ptr blit_pipeline;
  // TODO(conscat): Streamline descriptor sets.
  lava::descriptor::ptr shared_descriptor_set_layout;
  VkDescriptorSet shared_descriptor_set = VK_NULL_HANDLE;
  // crow::descriptor_layouts raytracing_descriptor_layouts;
  // crow::descriptor_sets raytracing_descriptor_sets;
  lava::descriptor::ptr raytracing_descriptor_set_layout;
  VkDescriptorSet raytracing_descriptor_set = VK_NULL_HANDLE;
  crow::descriptor_writes_stack descriptor_writes;
  lava::extras::raytracing::top_level_acceleration_structure::ptr top_as;
  lava::extras::raytracing::bottom_level_acceleration_structure::list
      bottom_as_list;

  lava::buffer::ptr scratch_buffer;
  VkDeviceAddress scratch_buffer_address = 0;
  lava::buffer::ptr raytracing_uniform_buffer;
  lava::image::ptr raytracing_output_image;

  // Recreate raytracing image and update its descriptors.
  lava::target_callback swapchain_callback = crow::create_swapchain_callback(
      app, raytracing_output_image, shared_descriptor_set, command_pool,
      graphics_queue);
  app.target->add_callback(&swapchain_callback);

  crow::entities entities;

  app.on_create = [&]() {
    lava::render_pass::ptr render_pass = app.shading.get_pass();

    // command pool for one-time command buffers
    const VkCommandPoolCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = uint32_t(graphics_queue.family)};
    if (!app.device->vkCreateCommandPool(&create_info, &command_pool)) {
      return false;
    }

    // uniform buffer for camera parameters and background color
    raytracing_uniform_buffer = lava::make_buffer();
    if (!raytracing_uniform_buffer->create_mapped(
            app.device, nullptr,
            app.target->get_frame_count() * raytracing_uniform_stride,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)) {
      return false;
    }

    // Output image for the raytracing shader.
    // RGBA16F is guaranteed to support these usage flags.
    VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
    raytracing_output_image = lava::make_image(format);
    raytracing_output_image->set_usage(
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    raytracing_output_image->set_layout(VK_IMAGE_LAYOUT_UNDEFINED);
    raytracing_output_image->set_aspect_mask(lava::format_aspect_mask(format));

    // descriptor set used by both the raytracing shaders and the blit shader
    shared_descriptor_set_layout = lava::make_descriptor();
    shared_descriptor_set_layout->add_binding(
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |
            VK_SHADER_STAGE_MISS_BIT_KHR);
    shared_descriptor_set_layout->add_binding(
        1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    if (!shared_descriptor_set_layout->create(app.device)) {
      return false;
    }

    // blit pipeline that draws the raytraced output image to the swapchain
    blit_pipeline_layout = lava::make_pipeline_layout();
    blit_pipeline_layout->add(shared_descriptor_set_layout);
    if (!blit_pipeline_layout->create(app.device)) {
      return false;
    }

    blit_pipeline = make_graphics_pipeline(app.device);

    if (!blit_pipeline->add_shader(
            lava::file_data(crow::find_shader_path("blit.vert.spv")),
            VK_SHADER_STAGE_VERTEX_BIT)) {
      return false;
    }

    if (!blit_pipeline->add_shader(
            lava::file_data(crow::find_shader_path("blit.frag.spv")),
            VK_SHADER_STAGE_FRAGMENT_BIT)) {
      return false;
    }

    blit_pipeline->add_color_blend_attachment();
    blit_pipeline->set_layout(blit_pipeline_layout);

    if (!blit_pipeline->create(render_pass->get())) {
      return false;
    }

    blit_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      const uint32_t uniform_offset =
          app.block.get_current_frame() * raytracing_uniform_stride;
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, blit_pipeline_layout->get(),
          0, 1, &shared_descriptor_set, 1, &uniform_offset);
      // fullscreen triangle
      app.device->call().vkCmdDraw(cmd_buf, 3, 1, 0, 0);
    };

    // add blit before lava's gui rendering
    render_pass->add_front(blit_pipeline);

    // descriptor used by the raytracing shader
    raytracing_descriptor_set_layout = lava::make_descriptor();
    raytracing_descriptor_set_layout->add_binding(
        0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    raytracing_descriptor_set_layout->add_binding(
        1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    raytracing_descriptor_set_layout->add_binding(
        2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    raytracing_descriptor_set_layout->add_binding(
        3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    if (!raytracing_descriptor_set_layout->create(app.device)) {
      return false;
    }

    raytracing_pipeline_layout = lava::make_pipeline_layout();
    raytracing_pipeline_layout->add(shared_descriptor_set_layout);
    raytracing_pipeline_layout->add(raytracing_descriptor_set_layout);
    if (!raytracing_pipeline_layout->create(app.device)) {
      return false;
    }

    raytracing_descriptor_set =
        raytracing_descriptor_set_layout->allocate(descriptor_pool->get());

    lava::VkVertexInputAttributeDescriptions vertex_attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal)},
    };

    /*
    std::vector<crow::shader_module> environment_shaders = {{
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/simple.vert.spv",
            .flags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        crow::shader_module{
            .file_name = get_exe_path() + "../../res/simple.frag.spv",
            .flags = VK_SHADER_STAGE_FRAGMENT_BIT,
        },
    }};

      // Global buffers:
    raytracing_descriptor_layouts[0] =
        crow::create_descriptor_layout(app, crow::global_descriptor_bindings);
    // Render-pass buffers:
    raytracing_descriptor_layouts[1] =
        crow::create_descriptor_layout(app, crow::simple_render_pass_bindings);
    // Material buffers:
    raytracing_descriptor_layouts[2] =
        crow::create_descriptor_layout(app, crow::simple_material_bindings);
    // Object buffers:
    raytracing_descriptor_layouts[3] = crow::create_descriptor_layout(
        app,
        {
            crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                     .binding_slot = 0,
                                     .descriptors_count = 1},
        });

    raytracing_descriptor_sets = crow::create_descriptor_sets(
        raytracing_descriptor_layouts, descriptor_pool);

    // TODO(conscat): Push to stack.
    VkWriteDescriptorSet const write_ubo_global{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = raytracing_descriptor_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = app.camera.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_pass{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = raytracing_descriptor_sets[1],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_material{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = raytracing_descriptor_sets[2],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_object{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = raytracing_descriptor_sets[3],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    app.device->vkUpdateDescriptorSets({write_ubo_global, write_ubo_pass,
                                        write_ubo_material, write_ubo_object});
    */

    // Create entities.
    lava::mesh::ptr player_mesh = lava::make_mesh();
    lava::mesh_data player_mesh_data =
        lava::create_mesh_data(lava::mesh_type::cube);
    player_mesh->add_data(player_mesh_data);
    player_mesh->create(app.device);
    entities.meshes[crow::entity::WORKER] = player_mesh;
    entities.initialize_transforms(app, crow::entity::WORKER,
                                   &raytracing_descriptor_sets,
                                   &descriptor_writes);
    entities.velocities[crow::entity::WORKER] = glm::vec3{0.1f, 0, 0};
    crow::update_descriptor_writes(app, &descriptor_writes);

    // Create pipelines.
    // raytracing_pipeline = crow::create_rasterization_pipeline(
    //     app, raytracing_pipeline_layout, environment_shaders,
    //     raytracing_descriptor_layouts, vertex_attributes);
    return true;
  };

  app.on_destroy = [&]() {
    // TODO(conscat): Free all arrays of lava objects.
    // raytracing_descriptor_layouts[0]->destroy();
    // raytracing_descriptor_layouts[3]->destroy();
    descriptor_pool->destroy();
    raytracing_pipeline->destroy();
    raytracing_pipeline_layout->destroy();
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
    ImVec2 minimap_size = {wh.x / 1920, wh.y / 1080};
    ImGui::SetNextWindowPos(minimap_window_xy, ImGuiCond_Always);
    ImGui::SetNextWindowSize(minimap_window_wh, ImGuiCond_Always);
    // finally create the window
    ImGui::Begin("Facility Map", 0,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoTitleBar);

    lava::mouse_position_ref _mouse_pos = app.input.get_mouse_position();
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      // first, check to see if the mouse click began inside of the minimap
      // window
      if (_mouse_pos.x < minimap_window_xy.x + minimap_window_wh.x &&
          _mouse_pos.y < minimap_window_xy.y + minimap_window_wh.y &&
          _mouse_pos.x > minimap_window_xy.x &&
          _mouse_pos.y > minimap_window_xy.y) {
        // set all necessary flags and variables for potential mouse dragging

        // set the mouse position to the minimap, used for when we start
        // dragging the map or whatever
        minimap.mouse_position = {
            (float)((_mouse_pos.x - minimap_window_xy.x) / minimap_window_wh.x),
            (float)((_mouse_pos.y - minimap_window_xy.y) /
                    minimap_window_wh.y)};
        printf("i have confirmed the click \n");
      }
    }
    // processing for dragging the minimap around all goes in here
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
      // first check to see that we have "grabbed" the minimap with the mouse.
      // if it has been grabbed, then minimap.mouse_position.x should have been
      // set to a float between 0:1
      if (minimap.mouse_position.x != -1) {
        // position of the mouse on this tick.
        ImVec2 current_frame__mouse_position = {
            (float)((_mouse_pos.x - minimap_window_xy.x) / minimap_window_wh.x),
            (float)((_mouse_pos.y - minimap_window_xy.y) /
                    minimap_window_wh.y)};
        // difference of the position of the mouse on this tick and the last
        // tick
        ImVec2 mouse_position_difference = {
            current_frame__mouse_position.x - minimap.mouse_position.x,
            current_frame__mouse_position.y - minimap.mouse_position.y};
        // the mouse position is stored in a range of 0:1 rather than absolute
        // position to have compatibility with the window randomly being resized
        // for no reason at all, so we need to convert it to a range that the
        // minimap understands
        mouse_position_difference.x *= 1920 * 0.4f;
        mouse_position_difference.y *= 1080 * 0.3f;
        // add this difference to the position of the minimap
        minimap.cpos.x += mouse_position_difference.x;
        minimap.cpos.y += mouse_position_difference.y;
        // set last frame's position to this frame's position since we are done
        // processing for it
        minimap.mouse_position = current_frame__mouse_position;
        // drag boolean predominantly used to ensure that room click processing
        // is not called when releasing the mouse button after dragging the
        // minimap around
        minimap.dragging = true;

        // this makes sure that the minimap doesnt get dragged way out of bounds
        // or anything like that
        minimap.cpos.x =
            std::clamp(minimap.cpos.x, minimap.mpos.x, minimap.mpos.z);
        minimap.cpos.y =
            std::clamp(minimap.cpos.y, minimap.mpos.y, minimap.mpos.w);
      }
    }

    // begin drawing the rooms to imgui, one by one
    for (int i = 0; i < minimap.rooms.size(); ++i) {
      // check for out of bounds rooms to skip adding them to draw calls
      // in order, the checks are:
      // 1) check to see if the room is wholly left of the map
      // 2) check to see if the room is wholly above the map
      // 3) check to see if the room is wholly right of the map
      // 4) check to see if the room is wholly under the map
      if (minimap.rooms[i].x + minimap.rooms[i].z + minimap.cpos.x < 0 ||
          minimap.rooms[i].y + minimap.rooms[i].w + minimap.cpos.y < 0 ||
          minimap.rooms[i].x + minimap.cpos.x > 1920 * 0.4f ||
          minimap.rooms[i].y + minimap.cpos.y > 1080 * 0.3f)
        // (768, 324)
        continue;
      // the room has passed cull checks, draw them

      // sets the x, y position of the room
      ImGui::SetCursorPos(
          {(minimap.rooms[i].x + minimap.cpos.x) * minimap_size.x,
           (minimap.rooms[i].y + minimap.cpos.y) * minimap_size.y});
      // debug stuff
      char room[] = {"##roomX"};
      sprintf(room, "##room%i", i);
      if (ImGui::Button(room, {minimap.rooms[i].z * minimap_size.x,
                               minimap.rooms[i].w * minimap_size.y})) {
        if (minimap.dragging == false) {
          // printf for testing
          printf("clicked on room %i\n", i);
          /* processing for room switch goes here */
        }
      }
    }

    // this must be called after all processing for the minimap because it
    // resets variables that may be used while processing the minimap
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      minimap.mouse_position = {-1, -1};
      minimap.dragging = false;
    }
    // paranoia check: using !ismousedown instead of mousereleased
    ImGui::End();
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

    raytracing_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          raytracing_pipeline_layout->get(), 0, 4,
          raytracing_descriptor_sets.data(), 0, nullptr);
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
    // for (auto& meshes : meshes) {
    //   if (meshes) {
    //     meshes->destroy();
    //   }
    // }
  });

  return app.run();
}
