#pragma once

#include <liblava-extras/raytracing.hpp>
#include <liblava/app.hpp>

#include "geometry.hpp"

namespace crow {

enum rt_stage : unsigned {
  // this reflects the order they're added in above
  raygen = 0,
  miss,
  closest_hit,
  callable
};

template <typename VertexType = lava::vertex>
auto create_rasterization_pipeline(
    lava::app& app, lava::pipeline_layout::ptr& pipeline_layout,
    std::vector<crow::shader_module>& shader_modules,
    crow::descriptor_layouts& descriptor_layouts,
    lava::VkVertexInputAttributeDescriptions& vertex_attributes,
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    -> lava::graphics_pipeline::ptr;

template <typename VertexType = lava::vertex>
auto create_raytracing_pipeline(
    lava::app& app, lava::pipeline_layout::ptr& pipeline_layout,
    std::vector<crow::shader_module>& shader_modules,
    crow::descriptor_layouts& descriptor_layouts)
    -> lava::extras::raytracing::raytracing_pipeline::ptr;

#include "../cpp/pipeline.tpp"

auto create_descriptor_pool(lava::app& app) -> lava::descriptor::pool::ptr;

auto create_descriptor_layout(lava::app& app,
                              std::vector<crow::descriptor_binding> bindings)
    -> lava::descriptor::ptr;

auto create_descriptor_sets(crow::descriptor_layouts& layouts,
                            lava::descriptor::pool::ptr& pool)
    -> crow::descriptor_sets;

void update_descriptor_writes(lava::app& app,
                              crow::descriptor_writes_stack& descriptors);

auto create_shader_binding_table(
    lava::extras::raytracing::raytracing_pipeline::ptr const& pipeline)
    -> lava::extras::raytracing::shader_binding_table::ptr;

}  // namespace crow
