#include <liblava/lava.hpp>

#include <iostream>

#include "../debug_camera_control/debug_camera_control.hpp"

fn main()->int {
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

  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;
  world_matrix_buffer.create_mapped(app.device, &world_matrix_buffer_data,
                                    sizeof(world_matrix_buffer_data),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  lava::mesh::ptr cube;
  cube = create_mesh(app.device, lava::mesh_type::cube);

  lava::descriptor::ptr descriptor_layout;
  lava::descriptor::pool::ptr descriptor_pool;
  VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

  lava::graphics_pipeline::ptr pipeline;
  lava::pipeline_layout::ptr layout;

  app.on_create = [&]() {
    pipeline = make_graphics_pipeline(app.device);
    pipeline->add_color_blend_attachment();
    pipeline->set_depth_test_and_write();
    pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS_OR_EQUAL);

    // All shapes use the same simple shaders
    if (!pipeline->add_shader(lava::file_data("../res/simple.vert.spv"),
                              VK_SHADER_STAGE_VERTEX_BIT)) {
      return false;
    }
    if (!pipeline->add_shader(lava::file_data("../res/simple.frag.spv"),
                              VK_SHADER_STAGE_FRAGMENT_BIT)) {
      return false;
    }

    pipeline->set_vertex_input_binding(
        {0, sizeof(lava::vertex), VK_VERTEX_INPUT_RATE_VERTEX});
    pipeline->set_vertex_input_attributes({
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal)},
    });

    descriptor_layout = lava::make_descriptor();
    descriptor_layout->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   VK_SHADER_STAGE_VERTEX_BIT);  // View matrix
    descriptor_layout->add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   VK_SHADER_STAGE_VERTEX_BIT);  // World matrix
    descriptor_layout->add_binding(
        2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT);  // Rotation vector
    descriptor_layout->create(app.device);

    descriptor_pool = lava::make_descriptor_pool();
    descriptor_pool->create(app.device,
                            {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3}});

    layout = lava::make_pipeline_layout();
    layout->add(descriptor_layout);
    layout->create(app.device);
    pipeline->set_layout(layout);

    descriptor_set = descriptor_layout->allocate(descriptor_pool->get());
    VkWriteDescriptorSet const write_ubo_camera{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = app.camera.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_world{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    VkWriteDescriptorSet const write_ubo_object{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 2,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
    };
    app.device->vkUpdateDescriptorSets(
        {write_ubo_camera, write_ubo_world, write_ubo_object});

    lava::render_pass::ptr render_pass = app.shading.get_pass();

    pipeline->create(render_pass->get());
    render_pass->add_front(pipeline);
    pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
      layout->bind(cmd_buf, descriptor_set);
      cube->bind_draw(cmd_buf);
    };

    return true;
  };

  app.on_destroy = [&]() {
    descriptor_layout->free(descriptor_set, descriptor_pool->get());
    descriptor_pool->destroy();
    descriptor_layout->destroy();
    pipeline->destroy();
    layout->destroy();
  };

  app.on_update = [&](lava::delta /*dt*/) {
    memcpy(lava::as_ptr(world_matrix_buffer.get_mapped_data()),
           &world_matrix_buffer_data, sizeof(world_matrix_buffer_data));

    if (app.camera.activated()) {
      app.camera.update_view(lava::to_dt(app.run_time.delta),
                             app.input.get_mouse_position());
    }

    return true;
  };

  app.add_run_end([&]() { cube->destroy(); });

  return app.run();
}
