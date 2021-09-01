#include <liblava-extras/raytracing.hpp>
#include <liblava/app.hpp>

#include "../hpp/geometry.hpp"

template <typename VertexType>
auto create_rasterization_pipeline(
    lava::app& app, lava::pipeline_layout::ptr& pipeline_layout,
    std::vector<crow::shader_module>& shader_modules,
    crow::descriptor_layouts& descriptor_layouts,
    lava::VkVertexInputAttributeDescriptions& vertex_attributes,
    VkPrimitiveTopology topology) -> lava::graphics_pipeline::ptr {
  lava::graphics_pipeline::ptr pipeline =
      lava::make_graphics_pipeline(app.device);
  for (auto& shader : shader_modules) {
    pipeline->add_shader(lava::file_data(shader.file_name), shader.flags);
  }

  pipeline->set_input_topology(topology);
  pipeline->add_color_blend_attachment();
  pipeline->set_depth_test_and_write();
  pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS_OR_EQUAL);
  pipeline->set_vertex_input_binding(
      {0, sizeof(VertexType), VK_VERTEX_INPUT_RATE_VERTEX});
  pipeline->set_vertex_input_attributes(vertex_attributes);

  pipeline_layout = lava::make_pipeline_layout();
  for (auto& descriptor_layout : descriptor_layouts) {
    pipeline_layout->add_descriptor(descriptor_layout);
  }
  pipeline_layout->create(app.device);
  pipeline->set_layout(pipeline_layout);
  lava::render_pass::ptr render_pass = app.shading.get_pass();
  pipeline->create(render_pass->get());
  render_pass->add_front(pipeline);
  return pipeline;
}

template <typename VertexType>
auto create_raytracing_pipeline(
    lava::app& app, lava::pipeline_layout::ptr& pipeline_layout,
    std::vector<crow::shader_module>& shader_modules,
    crow::descriptor_layouts& descriptor_layouts)
    -> lava::extras::raytracing::raytracing_pipeline::ptr {
  lava::extras::raytracing::raytracing_pipeline::ptr pipeline =
      lava::extras::raytracing::make_raytracing_pipeline(app.device);
  for (auto& shader : shader_modules) {
    pipeline->add_shader(lava::file_data(shader.file_name), shader.flags);
  }
  return nullptr;
}
