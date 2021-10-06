#include "../hpp/descriptor_setup.hpp"

#include "../hpp/geometry.hpp"

namespace crow {
void set_desc_layouts(crow::descriptor_layouts& desc_layouts, lava::app* app) {
  // desc_layouts.resize(desc_layouts.size() + num);

  // for (size_t i = num; i < desc_layouts.size(); ++i) {
  // if (num < desc_layouts.size()) {
  desc_layouts[0] =
      crow::create_descriptor_layout(*app, crow::global_descriptor_bindings);
  // Render-pass buffers:
  desc_layouts[1] =
      crow::create_descriptor_layout(*app, crow::simple_render_pass_bindings);
  // Material buffers:
  desc_layouts[2] =
      crow::create_descriptor_layout(*app, crow::simple_material_bindings);
  // Object buffers:
  desc_layouts[3] = crow::create_descriptor_layout(
      *app,
      {
          crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                                   .binding_slot = 0,
                                   .descriptors_count = 1},
      });
  //}
  //}
}

void create_desc_sets(crow::descriptor_layouts& desc_layouts,
                      crow::descriptor_sets& desc_sets,
                      lava::descriptor::pool::ptr descriptor_pool) {
  // desc_sets.resize(desc_layouts.size());
  // for (size_t i = 0; i < desc_sets.size(); ++i) {
  desc_sets = crow::create_descriptor_sets(desc_layouts, descriptor_pool);
  // }
}

void update_desc_sets(crow::descriptor_sets& desc_sets, lava::app* app,
                      lava::buffer& camera_buffer) {
  lava::mat4 matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer matrix_buffer;
  matrix_buffer.create_mapped(app->device, &matrix_buffer_data,
                              sizeof(matrix_buffer_data),
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

  // for (size_t i = 0; i < desc_sets.size(); ++i) {
  VkWriteDescriptorSet const write_ubo_global{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = desc_sets[0],
      .dstBinding = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = camera_buffer.get_descriptor_info(),
  };
  VkWriteDescriptorSet const write_ubo_pass{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = desc_sets[1],
      .dstBinding = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = matrix_buffer.get_descriptor_info(),
  };
  VkWriteDescriptorSet const write_ubo_material{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = desc_sets[2],
      .dstBinding = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = matrix_buffer.get_descriptor_info(),
  };
  VkWriteDescriptorSet const write_ubo_object{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = desc_sets[3],
      .dstBinding = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = matrix_buffer.get_descriptor_info(),
  };
  app->device->vkUpdateDescriptorSets(
      {write_ubo_global, write_ubo_pass, write_ubo_material, write_ubo_object});
  //}
}

}  // namespace crow