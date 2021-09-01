#pragma once

#include <liblava/app.hpp>

#include "entities.hpp"

namespace crow {

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

}  // namespace crow
