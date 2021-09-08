#pragma once

#include <liblava/lava.hpp>

#include "../hpp/component.hpp"
#include "geometry.hpp"

namespace crow {

struct entities {
  // NOLINTNEXTLINE
  std::vector<lava::mesh_data> meshes;
  // NOLINTNEXTLINE
  std::vector<glm::mat4x3> transforms_data;
  // TODO: Remove transforms pbuffer, since it is useless for raytracing.
  // NOLINTNEXTLINE
  std::vector<lava::buffer::ptr> transforms_pbuffer;
  // NOLINTNEXTLINE
  std::vector<glm::vec3> velocities;
  // NOLINTNEXTLINE
  std::vector<std::unique_ptr<crow::component_interface>> pcomponents;

  entities() {
    // Reserve space for the worker and sphynx.
    meshes.resize(2);
    transforms_data.resize(2);
    transforms_pbuffer.resize(2);
    velocities.resize(2);
    pcomponents.resize(2);
  }

  void initialize_transform(lava::app& app, size_t entity_index) {
    velocities[entity_index] = glm::vec3{0, 0, 0};
    transforms_data[entity_index] = glm::identity<glm::mat4x3>();
    transforms_pbuffer[entity_index] = lava::make_buffer();
    transforms_pbuffer[entity_index]->create_mapped(
        app.device, &transforms_data[entity_index],
        sizeof(transforms_data[entity_index]),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
  }

  void update_transform_data(size_t index, lava::delta dt) {
    transforms_data[index][3] += glm::vec3(velocities[index] * dt);
  }

  void update_transform_buffer(size_t index) {
    memcpy(lava::as_ptr(transforms_pbuffer[index]->get_mapped_data()),
           &transforms_data[index], sizeof(transforms_data[index]));
  }

  void free(size_t index) {}
};

enum entity {
  WORKER = 0,
  SPHYNX = 1,
};

}  // namespace crow
