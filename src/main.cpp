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
#include "hpp/enemy_behaviors.hpp"
#include "hpp/game_state.hpp"
#include "hpp/geometry.hpp"
#include "hpp/map.hpp"
#include "hpp/minimap.hpp"
#include "hpp/object.hpp"
#include "hpp/player_behavior.hpp"

auto main() -> int {
  // soloud sound initialization
  crow::audio::sound_loaded = false;
  crow::audio::initialize();

  // temporary
  ai_manager enemy_manager;

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
  lava::mesh::ptr current_room_mesh;

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
  std::string fbx_path =
      crow::get_exe_path() + "../../res/fbx/deer.fbx";  // Deer model
  ofbx::IScene* scene = lava::extras::load_fbx_scene(fbx_path.c_str());
  std::cout << "Loaded FBX scene.\n";

  lava::extras::fbx_data fbx_data = lava::extras::load_fbx_model(scene);
  // cube->add_data(fbx_data.mesh_data);
  // cube->create(app.device);

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

  crow::entities entities;

  // room buffer creation
  crow::descriptor_sets room_descriptor_sets;
  // crow::descriptor_writes_stack room_descriptor_writes;
  crow::descriptor_layouts room_descriptor_layouts;

  // sphynx buffer creation
  crow::descriptor_sets s_desc_sets;
  // crow::descriptor_writes_stack s_desc_writes;
  crow::descriptor_layouts s_desc_layouts;

  // setting up the gamestate
  crow::game_state game_state;
  game_state.current_state = game_state.PLAYING;
  // points to important game data
  game_state.environment_descriptor_sets = &environment_descriptor_sets;
  game_state.enemy_descriptor_sets = &s_desc_sets;
  game_state.descriptor_writes = &descriptor_writes;
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

    VkWriteDescriptorSet const write_ubo_global_s{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = s_desc_sets[0],
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = camera_buffer.get_descriptor_info(),
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
        .pBufferInfo = camera_buffer.get_descriptor_info(),
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

    // Create entities.
    // lava::mesh::ptr player_mesh = lava::make_mesh();
    // lava::mesh_data player_mesh_data =
    //   lava::create_mesh_data(lava::mesh_type::cube);
    /* player_mesh->add_data(fbx_data.mesh_data);
     player_mesh->create(app.device);
     entities.meshes[crow::entity::WORKER] = player_mesh;*/
    crow::new_game(game_state);
    minimap.active_room->set_active(&app, current_room_mesh, app.camera);
    enemy_manager.set_current_room(minimap.active_room);
    enemy_manager.load_entity_data(*game_state.entities, crow::entity::SPHYNX,
                                   crow::entity::WORKER);
    enemy_manager.create_behavior_tree();
    // game_state.current_state = game_state.MAIN_MENU;

    // FOR BUILD PURPOSES ONLY, TO BE REMOVED
    entities.transforms_data[crow::entity::WORKER][3][0] = -7.0f;
    entities.transforms_data[crow::entity::WORKER][3][2] = -7.0f;

    entities.transforms_data[crow::entity::SPHYNX][3][0] = 7.0f;
    entities.transforms_data[crow::entity::SPHYNX][3][2] = 7.0f;
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
          // processing for left clicks while you are currently playing the game
          if (game_state.current_state == game_state.PLAYING) {
            // crow::audio::play_sfx(0);
            glm::vec3 mouse_point = crow::mouse_to_floor(&app);
            // if mouse_point.y == -1 then the mouse is not pointing at the
            // floor
            if (mouse_point.y != -1) {
              std::vector<glm::vec2> temporary_results =
                  minimap.active_room->get_path(
                      glm::vec2(
                          entities.transforms_data[crow::entity::WORKER][3][0],
                          entities.transforms_data[crow::entity::WORKER][3][2]),
                      glm::vec2(mouse_point.x, mouse_point.z));

              if (temporary_results.size()) {
                // if the clicked position is the same as the previous position,
                // then we can assume that you've double clicked. thus, the
                // worker should run instead of walk
                if (crow::path_result.size() &&
                    crow::path_result[0] == temporary_results[0]) {
                  // check to ensure that the clicks were close enough to each
                  // other to count as a double click. if not, then nothing
                  // should happen since the worker is always walking towards
                  // the clicked destination
                  if (game_state.left_click_time < 0.5f) {
                    // worker starts running to destination
                    crow::worker_speed = crow::worker_run_speed;

                    // plays footstep sound when worker moves
                    crow::audio::add_footstep_sound(
                        &entities.transforms_data[crow::WORKER], 0.285f);
                  }
                } else {
                  // worker starts walking to destination
                  crow::worker_speed = crow::worker_walk_speed;

                  // plays footstep sound when worker moves
                  crow::audio::add_footstep_sound(
                      &entities.transforms_data[crow::WORKER], 0.5f);
                }
              }

              // set the worker's path
              crow::path_result = temporary_results;
            }
          }
          game_state.left_click_time = 0;

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
    crow::path_through(entities, crow::entity::WORKER, crow::worker_speed, dt);
    // could be move to on click
    enemy_manager.set_current_room(minimap.active_room);

    enemy_manager.update_position(*game_state.entities, crow::entity::SPHYNX);
    enemy_manager.update_target_position(*game_state.entities,
                                         crow::entity::WORKER);
    status b_tree_result = enemy_manager.b_tree.run();

    for (size_t i = 0; i < entities.transforms_data.size(); i++) {
      entities.update_transform_data(i, dt);
      entities.update_transform_buffer(i);
    }

    /* game_state.entities->velocities[crow::entity::SPHYNX] =
     *enemy_manager.velocity;*/
    /* std::cout << "sphynx velocity x: " << enemy_manager.velocity->x
               << " z: " << enemy_manager.velocity->z << "\n";

     std::cout << "sphynx pos x:"
               << entities.transforms_data[crow::entity::SPHYNX][3][0] << " z: "
               << entities.transforms_data[crow::entity::SPHYNX][3][2] <<
     "\n";*/

    if (game_state.current_state == game_state.PLAYING) {
      app.camera.update_view(dt, app.input.get_mouse_position());
      camera_buffer_data.projection_view = app.camera.get_view_projection();
      memcpy(camera_buffer.get_mapped_data(), &camera_buffer_data,
             sizeof(camera_buffer_data));

      // time elapsed since last left click
      game_state.left_click_time += dt;
      // updates sound timer objects
      crow::audio::update_audio_timers(&game_state, dt);
    }

    // actual game loop; execute the entire game by simply cycling through
    // the array of objects

    environment_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4, room_descriptor_sets.data(),
          0, nullptr);
      if (/*current_room_mesh*/ minimap.active_room->room_mesh) {
        minimap.active_room->room_mesh->bind_draw(cmd_buf);
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
      entities.meshes[crow::entity::WORKER]->bind_draw(cmd_buf);

      app.device->call().vkCmdBindDescriptorSets(
          cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
          environment_pipeline_layout->get(), 0, 4, s_desc_sets.data(), 0,
          nullptr);
      entities.meshes[crow::entity::SPHYNX]->bind_draw(cmd_buf);

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

    // temp_position = crow::get_floor_point(app.camera);
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
