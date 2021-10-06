#pragma once

#include <liblava/app.hpp>

#include <tuple>

#include "./geometry.hpp"

namespace crow {

auto create_descriptor_layout(lava::app& app,
                              std::vector<crow::descriptor_binding> bindings)
    -> lava::descriptor::ptr;

auto create_descriptor_sets(crow::descriptor_layouts& layouts,
                            lava::descriptor::pool::ptr& pool)
    -> crow::descriptor_sets;

template <typename VertexType = lava::vertex>
auto create_rasterization_pipeline(
    lava::app& app, lava::pipeline_layout::ptr& pipeline_layout,
    std::vector<shader_module>& shader_modules,
    crow::descriptor_layouts& descriptor_layouts,
    lava::VkVertexInputAttributeDescriptions& vertex_attributes,
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    -> lava::graphics_pipeline::ptr;

void update_descriptor_writes(lava::app* app,
                              crow::descriptor_writes_stack* descriptors);

#include "../cpp/pipeline.tpp"

}  // namespace crow
