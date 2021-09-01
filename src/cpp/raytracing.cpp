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

auto create_swapchain_callback(lava::app& app, VkDescriptorSet& descriptor_set,
                               crow::raytracing_uniform_data& uniforms,
                               lava::image::ptr output_image,
                               VkCommandPool& pool, lava::queue::ref& queue)
    -> lava::target_callback {
  // catch swapchain recreation
  // recreate raytracing image and update its descriptors
  lava::target_callback swapchain_callback;
  swapchain_callback.on_created = [&](lava::VkAttachmentsRef /*unused*/,
                                      lava::rect area) {
    const glm::uvec2 size = area.get_size();
    uniforms.inv_proj =
        glm::inverse(lava::perspective_matrix(size, 90.0f, 5.0f));
    uniforms.viewport = {area.get_origin(), size};

    if (!output_image->create(app.device, size)) {
      return false;
    }

    // update image descriptor
    const VkDescriptorImageInfo image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = output_image->get_view(),
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL};
    const VkWriteDescriptorSet write_info = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 1,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &image_info};
    app.device->vkUpdateDescriptorSets({write_info});

    // transition image to general layout
    return one_time_command_buffer(
        app.device, pool, queue, [&](VkCommandBuffer cmd_buf) {
          insert_image_memory_barrier(
              app.device, cmd_buf, output_image->get(), 0,
              VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
              VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
              VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
              output_image->get_subresource_range());
        });
  };

  swapchain_callback.on_destroyed = [&]() {
    app.device->wait_for_idle();
    output_image->destroy();
  };

  return swapchain_callback;
}

}  // namespace crow
