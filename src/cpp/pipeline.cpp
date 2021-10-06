#include "../hpp/pipeline.hpp"

#include <liblava/app.hpp>

#include <bit>
#include <tuple>

namespace crow {

auto create_descriptor_layout(lava::app& app,
                              std::vector<crow::descriptor_binding> bindings)
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

// TODO(conscat): Non-moving objects should have a different descriptor layout
// and pipeline.

void update_descriptor_writes(lava::app* app,
                              crow::descriptor_writes_stack* descriptors) {
  while (!descriptors->empty()) {
    app->device->vkUpdateDescriptorSets({descriptors->top()});
    descriptors->pop();
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
  // TODO: fix
  // NOLINTNEXTLINE
  return descriptor_sets;
}

}  // namespace crow
