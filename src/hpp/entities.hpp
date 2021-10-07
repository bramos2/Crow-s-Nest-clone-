#pragma once

#include <liblava/lava.hpp>

#include "../../src/hpp/descriptor_setup.hpp"
#include "../../src/hpp/geometry.hpp"

namespace crow {

struct entities {
  // NOLINT
  std::vector<lava::mesh::ptr> meshes;
  // NOLINT
  std::vector<glm::mat4> transforms_data;
  // NOLINT
  std::vector<lava::buffer::ptr> transforms_pbuffer;
  // NOLINT
  std::vector<glm::vec3> velocities;
  // NOLINT
  std::vector<crow::descriptor_layouts> desc_layouts;
  // NOLINT
  std::vector<crow::descriptor_sets> desc_sets_list;
  //push t buffers here when they are no longer needed for now. Destroying them causes crash.
  // NOLINT
  std::vector<lava::buffer::ptr> transforms_buffer_trash;
  // std::vector<std::unique_ptr<crow::component_interface>> pcomponents;
  size_t current_size = 0;
  entities() {
    // Reserve space for the worker and sphynx.
    // TODO(conscat): Reserve space for other entities.
    current_size = 2;
    meshes.resize(2);
    transforms_data.resize(2);
    transforms_pbuffer.resize(2);
    velocities.resize(2);
    desc_sets_list.resize(2);
    desc_layouts.resize(2);
    // pcomponents.resize(2);
  }

  void pop_back() {
    --current_size;
    meshes.back() = nullptr;
    meshes.pop_back();

    velocities.pop_back();
    for (size_t i = 0; i < 4; i++) {
      desc_layouts.back()[i]->destroy();
    }
    desc_layouts.pop_back();
    desc_sets_list.pop_back();
    // TODO: find a way to clean the buffers
    transforms_buffer_trash.push_back(transforms_pbuffer.back());
    transforms_pbuffer.pop_back();
    transforms_data.pop_back();
   
  }

  void initialize_entity(lava::app* app, size_t const index,
                         lava::mesh::ptr& mesh,
                         crow::descriptor_writes_stack* writes_stack,
                         lava::descriptor::pool::ptr descriptor_pool,
                         lava::buffer& camera_buffer) {
    if (index < current_size) {
      meshes[index] = mesh;

      crow::set_desc_layouts(desc_layouts[index], app);
      crow::create_desc_sets(desc_layouts[index], desc_sets_list[index],
                             descriptor_pool);
      crow::update_desc_sets(desc_sets_list[index], app, camera_buffer);

      velocities[index] = glm::vec3{0, 0, 0};
      transforms_data[index] = glm::mat4(1);  // Identity matrix.
      transforms_pbuffer[index] = lava::make_buffer();
      transforms_pbuffer[index]->create_mapped(
          app->device, &transforms_data[index], sizeof(transforms_data[index]),
          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
      writes_stack->push(VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .dstSet = (desc_sets_list[index])[3],  // [3] is the per-object
                                                 // buffer.
          .dstBinding = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .pBufferInfo = transforms_pbuffer[index]->get_descriptor_info(),
      });
    }
  }

  void update_transform_data(size_t const index, lava::delta dt) {
    if (index < current_size) {
      transforms_data[index][3] += glm::vec4(velocities[index] * dt, 0);
    }
  }

  void update_transform_buffer(size_t const index) {
    memcpy(lava::as_ptr(transforms_pbuffer[index]->get_mapped_data()),
           &transforms_data[index], sizeof(transforms_data[index]));
  }

  void set_world_position(size_t const index, float const x, float const y,
                          float const z) {
    if (index < current_size) {
      transforms_data[index][3][0] = x;
      transforms_data[index][3][1] = y;
      transforms_data[index][3][2] = z;
    }
  }

  void free(size_t index) {}

  void allocate(size_t amount) {
    current_size += amount;

    meshes.resize(current_size);
    transforms_data.resize(current_size);
    transforms_pbuffer.resize(current_size);
    velocities.resize(current_size);
    desc_sets_list.resize(current_size);
    desc_layouts.resize(current_size);
  }
};

// NOLINTNEXTLINE
enum class entity2 {
  WORKER = 0,
  SPHYNX = 1,
};

}  // namespace crow