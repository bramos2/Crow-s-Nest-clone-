#pragma once
#include <liblava/block.hpp>
#include <liblava/resource.hpp>

#include <stack>
#include <vector>

namespace crow {
// TODO(conscat): Could these go somewhere better?
struct shader_module {
  std::string file_name;
  VkShaderStageFlagBits flags;
};

using pipeline_layouts = std::vector<lava::pipeline>;

using descriptor_writes_stack = std::stack<VkWriteDescriptorSet>;

struct descriptor_binding {
  VkDescriptorType const type;
  VkShaderStageFlagBits const stage_flags;
  lava::index const binding_slot;
  lava::ui32 const descriptors_count;
};

auto const vertex_frag_bit = static_cast<VkShaderStageFlagBits>(
    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

std::vector<descriptor_binding> const global_descriptor_bindings = {
    crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                             .binding_slot = 0,
                             .descriptors_count = 1},
};

std::vector<descriptor_binding> const simple_render_pass_bindings = {
    crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
                             .binding_slot = 0,
                             .descriptors_count = 1},
};

std::vector<descriptor_binding> const simple_material_bindings = {
    crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             .stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT,
                             .binding_slot = 0,
                             .descriptors_count = 1},
};

std::vector<descriptor_binding> const simple_object_bindings = {
    crow::descriptor_binding{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             .stage_flags = VK_SHADER_STAGE_VERTEX_BIT,
                             .binding_slot = 0,
                             .descriptors_count = 1},
};

using descriptor_layouts = std::array<lava::descriptor::ptr, 4>;

// [0] Global buffers
// [1] Per-render pass buffers
// [2] Per-material buffers
// [3] Per-object buffers
using descriptor_sets = std::array<VkDescriptorSet, 4>;

template <typename T = lava::vertex>
auto make_floor(float x, float y, float w = 1, float h = 1)
    -> lava::mesh_template_data<T>;

template <typename T = lava::vertex>
auto create_floor(float x, float y, float w = 1, float h = 1)
    -> std::shared_ptr<lava::mesh_template<T>>;

#include "../cpp/geometry.tpp"

}  // namespace crow
