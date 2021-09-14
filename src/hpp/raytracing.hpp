#pragma once

#include <liblava-extras/raytracing.hpp>
#include <liblava/app.hpp>

#include "entities.hpp"

namespace crow {

enum raytracing_stage : std::uint32_t {
  // this reflects the order they're added in above
  RAYGEN = 0,
  MISS,
  CLOSEST_HIT,
  CALLABLE
};

struct raytracing_uniform_data {
  glm::mat4 inv_view;
  glm::mat4 inv_proj;
  glm::uvec4 viewport;
  glm::vec4 background_color;
  uint32_t max_depth;
};

struct raytracing_data;

struct instance_data {
  std::uint32_t vertex_base;
  std::uint32_t vertex_count;
  std::uint32_t index_base;
  std::uint32_t index_count;

  void add_data(crow::raytracing_data&, std::vector<lava::mesh_data>& meshes);
};

struct raytracing_data {
  std::vector<crow::instance_data> instances;
  std::vector<lava::vertex> vertices;
  std::vector<lava::index> indices;
};

auto create_swapchain_callback(lava::app& app, VkDescriptorSet& descriptor_set,
                               crow::raytracing_uniform_data& uniforms,
                               lava::image::ptr output_image,
                               VkCommandPool& pool, lava::queue::ref& queue)
    -> lava::target_callback;

auto create_acceleration_structure(lava::app& app, crow::raytracing_data& data,
                                   lava::buffer::ptr& vertex_buffer,
                                   lava::buffer::ptr& index_buffer,
                                   VkCommandPool& pool, lava::queue::ref& queue)
    -> lava::extras::raytracing::top_level_acceleration_structure::ptr;

void push_raytracing_descriptor_writes(
    crow::descriptor_writes_stack& writes_stack, size_t const& uniform_stride,
    lava::buffer::ptr& uniform_buffer, lava::buffer::ptr& vertex_buffer,
    lava::buffer::ptr& index_buffer, lava::buffer::ptr& instance_buffer,
    VkDescriptorSet& shared_descriptor_set,
    lava::extras::raytracing::top_level_acceleration_structure::ptr& top_as,
    VkDescriptorSet& raytracing_descriptor_set);

auto create_raytracing_image() -> lava::image::ptr;

}  // namespace crow
