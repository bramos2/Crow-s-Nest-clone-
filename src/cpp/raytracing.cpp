#include "../hpp/raytracing.hpp"

namespace crow {

void instance_data::add_data(crow::raytracing_data& arrays,
                             std::vector<lava::mesh_data>& meshes) {
  // We probably want to have the `i` index for something at somepoint.
  for (size_t i = 0; i < meshes.size(); i++) {  // NOLINT
    instance_data const instance = {
        .vertex_base = std::uint32_t(arrays.vertices.size()),
        .vertex_count = std::uint32_t(meshes[i].vertices.size()),
        .index_base = std::uint32_t(arrays.indices.size()),
        .index_count = std::uint32_t(meshes[i].indices.size())};
    arrays.instances.push_back(instance);
    arrays.vertices.insert(arrays.vertices.end(), meshes[i].vertices.begin(),
                           meshes[i].vertices.end());
    arrays.indices.insert(arrays.indices.end(), meshes[i].indices.begin(),
                          meshes[i].indices.end());
  }
}

}  // namespace crow
