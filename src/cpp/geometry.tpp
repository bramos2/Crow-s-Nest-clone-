#include "liblava/resource/mesh.hpp"

template <typename T>
auto make_floor(float x, float y, float w, float h)
    -> lava::mesh_template_data<T> {
  lava::mesh_template_data<T> tile;
  // TODO(conscat): This will be problematic without `lava::vertex`.
  tile = lava::create_mesh_data<T, true, true, true, true, true, true>(
      lava::mesh_type::quad);
  // Make the vertical quad become lateral.
  // TODO(conscat): There should be a rotation function in liblava.
  tile.vertices[0].position.y = 0;
  tile.vertices[0].position.z = -1;
  tile.vertices[1].position.y = 0;
  tile.vertices[1].position.z = -1;
  tile.scale_vector({w, 1, h});
  tile.move({x, y, 0});
  return tile;
}

template <typename T>
auto create_floor(float x, float y, float w, float h)
    -> std::shared_ptr<lava::mesh_template<T>> {
  std::shared_ptr<lava::mesh_template<T>> mesh;
  mesh->add_data(make_floor<T>(x, y, w, h));
  return mesh;
}
