#pragma once

#include <liblava/app.hpp>

#include "entities.hpp"

namespace crow {

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

}  // namespace crow
