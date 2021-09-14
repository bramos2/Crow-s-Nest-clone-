#include <liblava-extras/fbx.hpp>
#include <liblava-extras/raytracing.hpp>
#include <liblava/lava.hpp>

#include <imgui.h>

#include "hpp/camera.hpp"
#include "hpp/device.hpp"
#include "hpp/pipeline.hpp"
#include "liblava/resource/mesh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <glm/gtc/color_space.hpp>

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

auto main(int argc, char *argv[]) -> int {
  // soloud sound initialization
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  lava::frame_config config;
  config.info.app_name = "Crow's Nest";
  config.cmd_line = {argc, argv};
  config.info.req_api_version = lava::api_version::v1_2;
  lava::app app(config);
  app.config.surface.formats = {
      VK_FORMAT_B8G8R8A8_SRGB,
  };
  {
    lava::device::ptr device = crow::create_logical_device(app.manager);
    app.device = device.get();
  }
  app.manager.on_create_param = [](lava::device::create_param &param) {};
  app.setup();

  crow::initialize_debug_camera(app.camera);
  crow::camera_device_data camera_buffer_data = {
      app.camera.get_view_projection(),
  };
  lava::buffer camera_buffer;
  camera_buffer.create_mapped(
      app.device, &camera_buffer_data, sizeof(camera_buffer_data),
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

  crow::entities entities;
  crow::minimap minimap({0.0f, 0.65f}, {0.4f, 0.35f});

  // TODO: Move raytracing initialization out of main.
  lava::queue::ref queue = app.device->graphics_queue();
  size_t const uniform_stride =
      lava::align_up(sizeof(crow::raytracing_uniform_data),
                     app.device->get_physical_device()
                         ->get_properties()
                         .limits.minUniformBufferOffsetAlignment);

  std::vector<lava::descriptor::ptr> shared_descriptor_layouts;
  VkDescriptorSet shared_descriptor_set = VK_NULL_HANDLE;

  lava::pipeline_layout::ptr blit_pipeline_layout;
  lava::graphics_pipeline::ptr blit_pipeline;

  lava::pipeline_layout::ptr raytracing_pipeline_layout;
  lava::extras::raytracing::raytracing_pipeline::ptr raytracing_pipeline;

  lava::extras::raytracing::shader_binding_table::ptr shader_binding;

  // These only hold a single index.
  std::vector<lava::descriptor::ptr> raytracing_descriptor_layouts;
  std::vector<VkDescriptorSet> raytracing_descriptor_sets;

  lava::extras::raytracing::top_level_acceleration_structure::ptr top_as;

  VkCommandPool command_pool = VK_NULL_HANDLE;
  auto descriptor_pool = crow::create_descriptor_pool(app);
  crow::descriptor_writes_stack descriptor_writes;

  lava::buffer::ptr scratch_buffer;
  VkDeviceAddress scratch_buffer_address = 0;
  lava::buffer::ptr instance_buffer = lava::make_buffer();
  lava::buffer::ptr vertex_buffer = lava::make_buffer();
  lava::buffer::ptr index_buffer = lava::make_buffer();
  lava::buffer::ptr uniform_buffer = lava::make_buffer();
  crow::raytracing_uniform_data uniform_data{};
  lava::image::ptr output_image = crow::create_raytracing_image();

  crow::raytracing_data raytracing_data{};
  struct callable_record_data {
    glm::vec3 direction = {0.0f, 0.0f, 1.0f};
  } callable_record;

  lava::target_callback swapchain_callback =
      crow::create_swapchain_callback(app, shared_descriptor_set, uniform_data,
                                      output_image, command_pool, queue);
  app.target->add_callback(&swapchain_callback);

  // setting up the gamestate
  crow::game_state game_state;
  game_state.current_state = game_state.PLAYING;
  game_state.minimap = &minimap;
  game_state.entities = &entities;
  game_state.app = &app;

  app.on_create = [&]() {
    lava::render_pass::ptr render_pass = app.shading.get_pass();

    lava::VkVertexInputAttributeDescriptions vertex_attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal)},
    };

    // uniform buffer for camera parameters and background color
    if (!uniform_buffer->create_mapped(
            app.device, nullptr, app.target->get_frame_count() * uniform_stride,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)) {
      return false;
    }

    shared_descriptor_layouts.resize(1);
    shared_descriptor_layouts[0] = lava::make_descriptor();
    shared_descriptor_layouts[0]->add_binding(
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR |
            VK_SHADER_STAGE_MISS_BIT_KHR);
    shared_descriptor_layouts[0]->add_binding(
        1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    shared_descriptor_layouts[0]->create(app.device);
    shared_descriptor_set =
        shared_descriptor_layouts[0]->allocate(descriptor_pool->get());

    // Blitting pipeline
    std::vector<crow::shader_module> simple_shaders = {{
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/vert.spv",
            .flags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/frag.spv",
            .flags = VK_SHADER_STAGE_FRAGMENT_BIT,
        },
    }};

    blit_pipeline = crow::create_rasterization_pipeline(
        app, blit_pipeline_layout, simple_shaders, shared_descriptor_layouts,
        vertex_attributes);
    // Blit the raytracing result to swapchain buffer.
    blit_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      std::uint32_t const uniform_offset =
          app.block.get_current_frame() * uniform_stride;
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, blit_pipeline_layout->get(),
          0, 1, &shared_descriptor_set, 1, &uniform_offset);
      // No vertex buffer. The verts are hard-coded in the shader.
      app.device->call().vkCmdDraw(cmd_buf, 3, 1, 0, 0);
    };

    // Raytracing
    raytracing_descriptor_layouts[0]->add_binding(
        0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
        VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    raytracing_descriptor_layouts[0]->add_binding(
        1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    raytracing_descriptor_layouts[0]->add_binding(
        2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    raytracing_descriptor_layouts[0]->add_binding(
        3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    raytracing_descriptor_layouts[0]->create(app.device);

    std::vector<crow::shader_module> raytracing_shader_modules = {{
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/gen.ray.spv",
            .flags = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
        },
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/miss.ray.spv",
            .flags = VK_SHADER_STAGE_MISS_BIT_KHR,
        },
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/hit.ray.spv",
            .flags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
        },
        crow::shader_module{
            .file_name = crow::get_exe_path() + "../../res/spv/call.ray.spv",
            .flags = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
        },
    }};

    raytracing_pipeline_layout = lava::make_pipeline_layout();
    raytracing_pipeline = crow::create_raytracing_pipeline(
        app, raytracing_pipeline_layout, raytracing_shader_modules,
        descriptor_pool, raytracing_descriptor_sets,
        raytracing_descriptor_layouts, shared_descriptor_layouts);

    auto shader_binding_table =
        crow::create_shader_binding_table(raytracing_pipeline);

    crow::descriptor_writes_stack raytracing_writes_stack;
    crow::push_raytracing_descriptor_writes(
        raytracing_writes_stack, uniform_stride, uniform_buffer, vertex_buffer,
        index_buffer, instance_buffer, shared_descriptor_set, top_as,
        raytracing_descriptor_sets[0]);
    crow::update_descriptor_writes(app, raytracing_writes_stack);

    glm::uvec2 size = app.target->get_size();

    uniform_data.inv_view = glm::inverse(glm::lookAtLH(
        glm::vec3(0.75f, 0.25f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)));
    uniform_data.inv_proj =
        glm::inverse(lava::perspective_matrix(size, 90.0f, 5.0f));
    uniform_data.viewport = {0, 0, size};
    uniform_data.background_color = {
        glm::convertSRGBToLinear(render_pass->get_clear_color()), 1.0f};
    uniform_data.max_depth = 5;

    swapchain_callback.on_created({}, {{0, 0}, size});

    // Create entities.
    lava::mesh_data player_mesh_data =
        lava::create_mesh_data(lava::mesh_type::cube);
    entities.meshes[crow::entity::WORKER] = player_mesh_data;
    crow::new_game(game_state);
    game_state.current_state = game_state.MAIN_MENU;

    return true;
  };

  app.on_destroy = [&]() {
    // TODO: Free all arrays of lava objects.
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
      game_state.current_state = game_state.PAUSED;
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

    // TODO: Re-enable this:
    // minimap.draw_call(&app, current_room_mesh);

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
      ImGui::DragFloat3("position##camera", (lava::r32 *)&app.camera.position,
                        0.01f);
      ImGui::DragFloat3("rotation##camera", (lava::r32 *)&app.camera.rotation,
                        0.1f);
      ImGui::DragFloat3("position##mouse-point", (lava::r32 *)&temp_position,
                        0.1f);
      ImGui::Spacing();
      ImGui::End();
    }
#endif
  };

  app.on_update = [&](lava::delta dt) {
    // Game state:
    if (game_state.current_state == game_state.PLAYING) {
      app.camera.update_view(dt, app.input.get_mouse_position());
      camera_buffer_data.projection_view = app.camera.get_view_projection();
      memcpy(camera_buffer.get_mapped_data(), &camera_buffer_data,
             sizeof(camera_buffer_data));
    }

    for (size_t i = 0; i < entities.pcomponents.size(); i++) {
    }
    entities.update_transform_data(crow::entity::WORKER, dt);
    entities.update_transform_buffer(crow::entity::WORKER);

    // Raytracing:
    // TODO: VMA_MEMORY_USAGE_GPU_ONLY
    // TODO: Extract out of this function.
    instance_buffer->create(
        app.device, raytracing_data.instances.data(),
        sizeof(crow::instance_data) * raytracing_data.instances.size(),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, false, VMA_MEMORY_USAGE_CPU_TO_GPU);
    vertex_buffer->create(
        app.device, raytracing_data.vertices.data(),
        sizeof(lava::vertex) * raytracing_data.vertices.size(),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        false, VMA_MEMORY_USAGE_CPU_TO_GPU);
    index_buffer->create(
        app.device, raytracing_data.indices.data(),
        sizeof(lava::index) * raytracing_data.indices.size(),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
        false, VMA_MEMORY_USAGE_CPU_TO_GPU);
    top_as = crow::create_acceleration_structure(
        app, raytracing_data, vertex_buffer, index_buffer, command_pool, queue);

    // TODO: Fix narrowing conversions.
    for (size_t i = 0; i < raytracing_data.instances.size(); i++) {
      glm::vec3 pos = {(2.0f * i - 1) * 0.5f, 0.0f, i * 0.5f};
      float angle = glm::radians(15.0f) * float(lava::to_sec(lava::now())) * i;
      glm::mat4 transform =
          glm::translate(glm::mat4(1.0f), pos) *
          glm::rotate(glm::mat4(1.0f), angle, {0.0f, 1.0f, 0.0});
      top_as->set_instance_transform(i, transform);
    }

    return true;
  };

  app.on_process = [&](VkCommandBuffer cmd_buf, lava::index frame) {
    std::uint32_t const uniform_offset = frame * uniform_stride;
    char *address =
        static_cast<char *>(uniform_buffer->get_mapped_data()) + uniform_offset;
    // NOLINTNEXTLINE Change when GCC supports std::bitcast<>()
    *reinterpret_cast<crow::raytracing_uniform_data *>(address) = uniform_data;

    // rebuild TLAS with new transformation matrices

    const VkPipelineStageFlags build =
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
    const VkPipelineStageFlags use =
        VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;

    // wait for the last trace
    app.device->call().vkCmdPipelineBarrier(cmd_buf, use, build, 0, 0, nullptr,
                                            0, nullptr, 0, nullptr);

    top_as->update(cmd_buf, scratch_buffer_address);

    // wait for update to finish before the next trace
    const VkMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR |
                         VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
        .dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR};
    app.device->call().vkCmdPipelineBarrier(cmd_buf, build, use, 0, 1, &barrier,
                                            0, nullptr, 0, nullptr);

    // wait for previous image reads
    app.device->call().vkCmdPipelineBarrier(
        cmd_buf, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, nullptr, 0, nullptr,
        0, nullptr);

    raytracing_pipeline->bind(cmd_buf);

    app.device->call().vkCmdBindDescriptorSets(
        cmd_buf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
        raytracing_pipeline_layout->get(), 0, 1, &shared_descriptor_set, 1,
        &uniform_offset);
    app.device->call().vkCmdBindDescriptorSets(
        cmd_buf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
        raytracing_pipeline_layout->get(), 1, 1, &raytracing_descriptor_sets[0],
        0, nullptr);

    // trace rays!

    const glm::uvec3 size = {uniform_data.viewport.z, uniform_data.viewport.w,
                             1};

    const VkStridedDeviceAddressRegionKHR raygen =
        shader_binding->get_raygen_region();
    app.device->call().vkCmdTraceRaysKHR(
        cmd_buf, &raygen, &shader_binding->get_miss_region(),
        &shader_binding->get_hit_region(),
        &shader_binding->get_callable_region(), size.x, size.y, size.z);

    // wait for trace to finish before reading the image
    insert_image_memory_barrier(
        app.device, cmd_buf, output_image->get(), VK_ACCESS_SHADER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        output_image->get_subresource_range());
  };

  app.add_run_end([&]() {
    crow::audio::cleanup();
    crow::end_game(game_state);

    instance_buffer->destroy();
    vertex_buffer->destroy();
    index_buffer->destroy();
    top_as->destroy();
  });

  return app.run();
}
