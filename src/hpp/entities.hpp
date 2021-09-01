#pragma once

#include <liblava/lava.hpp>

#include "../hpp/component.hpp"
#include "geometry.hpp"

namespace crow {

struct entities {
  // NOLINTNEXTLINE
  std::vector<lava::mesh_data> meshes;
  // NOLINTNEXTLINE
  std::vector<glm::mat4> transforms_data;
  //TODO: Remove transforms pbuffer, since it is useless for raytracing.
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

  void initialize_transforms(lava::app& app, size_t entity_index,
                             crow::descriptor_sets* descriptor_sets,
                             crow::descriptor_writes_stack* writes_stack) {
    velocities[entity_index] = glm::vec3{0, 0, 0};
    transforms_data[entity_index] = glm::mat4(1);  // Identity matrix.
    transforms_pbuffer[entity_index] = lava::make_buffer();
    transforms_pbuffer[entity_index]->create_mapped(
        app.device, &transforms_data[entity_index], sizeof(transforms_data[entity_index]),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    writes_stack->push(VkWriteDescriptorSet{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = (*descriptor_sets)[3], // [3] is the per-object buffer.
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = transforms_pbuffer[entity_index]->get_descriptor_info(),
    });
  }

  void update_transform_data(size_t index, lava::delta dt) {
    transforms_data[index][3] += glm::vec4(velocities[index] * dt, 0);
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
