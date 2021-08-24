#include "../hpp/geometry.hpp"

#include <liblava/app.hpp>

namespace crow {

auto create_swapchain_callback(::lava::app& app, lava::image::ptr& output_image,
                               VkDescriptorSet shared_descriptor_set,
                               VkCommandPool pool, lava::queue::ref queue)
    -> lava::target_callback {
  lava::target_callback swapchain_callback;

  swapchain_callback.on_created = [&](lava::VkAttachmentsRef /*unused*/,
                                      lava::rect area) {
    crow::raytracing_uniform_data uniforms{};
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
        .dstSet = shared_descriptor_set,
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
