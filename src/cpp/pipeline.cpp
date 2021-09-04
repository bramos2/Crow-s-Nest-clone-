#include "../hpp/pipeline.hpp"

#include <liblava/app.hpp>

namespace crow {

auto create_descriptor_pool(lava::app& app) -> lava::descriptor::pool::ptr {
  auto descriptor_pool = lava::make_descriptor_pool();
  descriptor_pool->create(
      app.device,
      {
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
          {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},  // UV textures.
          {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},           // Raytracing output.
          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1},
          {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
      },
      4);
  return descriptor_pool;
}

auto create_descriptor_layout(
    lava::app& app,
    std::vector<crow::descriptor_binding>
        bindings)  // NOLINT this cannot be a const reference because data is
                   // passed in at call site.
    -> lava::descriptor::ptr {
  lava::descriptor::ptr descriptor_layout;
  descriptor_layout = lava::make_descriptor();

  for (auto& binding : bindings) {
    lava::descriptor::binding::ptr current_binding =
        lava::make_descriptor_binding(binding.binding_slot);
    current_binding->set_type(binding.type);
    current_binding->set_stage_flags(binding.stage_flags);
    current_binding->set_count(binding.descriptors_count);
    descriptor_layout->add(current_binding);
  }
  descriptor_layout->create(app.device);
  return descriptor_layout;
}

void update_descriptor_writes(lava::app& app,
                              crow::descriptor_writes_stack& descriptors) {
  while (!descriptors.empty()) {
    app.device->vkUpdateDescriptorSets({descriptors.top()});
    descriptors.pop();
  }
}

auto create_descriptor_sets(crow::descriptor_layouts& layouts,
                            lava::descriptor::pool::ptr& pool)
    -> crow::descriptor_sets {
  crow::descriptor_sets descriptor_sets;
  // There are never more than 4 descriptor sets in a descriptor layout.
  for (size_t i = 0; i < 4; i++) {
    if (layouts[i] != VK_NULL_HANDLE) {
      descriptor_sets[i] = layouts[i]->allocate(pool->get());
    }
  }
  return descriptor_sets;
}

auto create_shader_binding_table(
    lava::extras::raytracing::raytracing_pipeline::ptr const& pipeline)
    -> lava::extras::raytracing::shader_binding_table::ptr {
  auto shader_binding = lava::extras::raytracing::make_shader_binding_table();
  struct callable_record_data {
    glm::vec3 direction = {0.0f, 0.0f, 1.0f};
  } callable_record;
  std::vector hit_records(pipeline->get_shader_groups().size(),
                          lava::cdata(nullptr, 0));
  hit_records[callable] =
      lava::cdata(&callable_record, sizeof(callable_record));
  shader_binding->create(pipeline, hit_records);
  return shader_binding;
}

}  // namespace crow
