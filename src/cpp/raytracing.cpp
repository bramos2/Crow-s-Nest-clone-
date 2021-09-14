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

auto create_swapchain_callback(lava::app& app,
                               VkDescriptorSet& shared_descriptor_set,
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
    uniforms.inv_proj = glm::inverse(lava::perspective_matrix(size, 90.f, 5.f));
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

auto create_acceleration_structure(lava::app& app, crow::raytracing_data& data,
                                   lava::buffer::ptr& vertex_buffer,
                                   lava::buffer::ptr& index_buffer,
                                   VkCommandPool& pool, lava::queue::ref& queue)
    -> lava::extras::raytracing::top_level_acceleration_structure::ptr {
  // A BLAS (bottom level) for each mesh.
  // One TLAS (top level) referencing all the BLAS.
  constexpr bool COMPACT_BLAS = true;
  auto top_as =
      lava::extras::raytracing::make_top_level_acceleration_structure();
  lava::extras::raytracing::bottom_level_acceleration_structure::list
      bottom_as_list;
  lava::buffer::ptr scratch_buffer;
  VkDeviceAddress scratch_buffer_address = 0;

  // buffer data, common to all BLAS
  const VkAccelerationStructureGeometryTrianglesDataKHR triangles = {
      .sType =
          VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR,
      .vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
      .vertexData = {vertex_buffer->get_address()},
      .vertexStride = sizeof(lava::vertex),
      .maxVertex = std::uint32_t(data.vertices.size()),
      .indexType = VK_INDEX_TYPE_UINT32,
      .indexData = {index_buffer->get_address()}};

  VkDeviceSize scratch_buffer_size = 0;

  for (auto& instance : data.instances) {
    // per-mesh sub-buffer region
    const VkAccelerationStructureBuildRangeInfoKHR range = {
        .primitiveCount = instance.index_count / 3,
        .primitiveOffset = static_cast<std::uint32_t>(
            instance.index_base * sizeof(lava::index)),  // this is in bytes
        .firstVertex = instance.vertex_base  // but this is an index...
    };

    auto bottom_as =
        lava::extras::raytracing::make_bottom_level_acceleration_structure();
    bottom_as->add_geometry(triangles, range, VK_GEOMETRY_OPAQUE_BIT_KHR);

    bottom_as->create(
        app.device,
        VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
            (COMPACT_BLAS
                 ? VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR
                 : 0));

    bottom_as_list.push_back(bottom_as);
    scratch_buffer_size =
        std::max(scratch_buffer_size, bottom_as->scratch_buffer_size());

    top_as->add_instance(bottom_as);
  }

  top_as->create(app.device,
                 VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
                     VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR);

  scratch_buffer_size =
      std::max(scratch_buffer_size, top_as->scratch_buffer_size());
  scratch_buffer = lava::make_buffer();
  scratch_buffer->create(app.device, nullptr, scratch_buffer_size,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                             VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR);
  scratch_buffer_address = scratch_buffer->get_address();

  // build BLAS and TLAS
  one_time_command_buffer(
      app.device, pool, queue, [&](VkCommandBuffer cmd_buf) {
        // barrier to wait for build to finish
        const VkMemoryBarrier barrier = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
                             VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,
            .dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
                             VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR};
        const VkPipelineStageFlags src =
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
        const VkPipelineStageFlags dst =
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

        for (auto& i : bottom_as_list) {
          i->build(cmd_buf, scratch_buffer_address);
          app.device->call().vkCmdPipelineBarrier(
              cmd_buf, src, dst, 0, 1, &barrier, 0, nullptr, 0, nullptr);
        }
        top_as->build(cmd_buf, scratch_buffer_address);
        app.device->call().vkCmdPipelineBarrier(
            cmd_buf, src, dst | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0,
            1, &barrier, 0, nullptr, 0, nullptr);
      });

  // compact BLAS
  // building must be finished to retrieve the compacted size, or
  // vkGetQueryPoolResults will time out

  if constexpr (COMPACT_BLAS) {
    std::vector<
        lava::extras::raytracing::bottom_level_acceleration_structure::ptr>
        compacted_bottom_as_list;

    one_time_command_buffer(
        app.device, pool, queue, [&](VkCommandBuffer cmd_buf) {
          const VkMemoryBarrier barrier = {
              .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
              .srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
                               VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,
              .dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
                               VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR};
          const VkPipelineStageFlags src =
              VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
          const VkPipelineStageFlags dst =
              VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

          for (size_t i = 0; i < bottom_as_list.size(); i++) {
            auto compacted_bottom_as = bottom_as_list[i]->compact(cmd_buf);
            compacted_bottom_as_list.push_back(
                std::dynamic_pointer_cast<
                    lava::extras::raytracing::
                        bottom_level_acceleration_structure>(
                    compacted_bottom_as));
            // update the TLAS with references to the new compacted BLAS since
            // their handles changed
            top_as->update_instance(i, compacted_bottom_as_list[i]);
          }
          app.device->call().vkCmdPipelineBarrier(
              cmd_buf, src, dst, 0, 1, &barrier, 0, nullptr, 0, nullptr);
          top_as->update(cmd_buf, scratch_buffer_address);
          app.device->call().vkCmdPipelineBarrier(
              cmd_buf, src, dst | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
              0, 1, &barrier, 0, nullptr, 0, nullptr);
        });
    bottom_as_list = compacted_bottom_as_list;
  }

  return top_as;
}

void push_raytracing_descriptor_writes(
    crow::descriptor_writes_stack& writes_stack, size_t const& uniform_stride,
    lava::buffer::ptr& uniform_buffer, lava::buffer::ptr& vertex_buffer,
    lava::buffer::ptr& index_buffer, lava::buffer::ptr& instance_buffer,
    VkDescriptorSet& shared_descriptor_set,
    lava::extras::raytracing::top_level_acceleration_structure::ptr& top_as,
    VkDescriptorSet& raytracing_descriptor_set) {
  VkDescriptorBufferInfo buffer_info = *uniform_buffer->get_descriptor_info();
  // for dynamic uniform buffers, range must be the bound size, not the total
  // buffer size
  buffer_info.range = uniform_stride;

  std::array<VkWriteDescriptorSet const, 5> const write_sets = {
      VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .dstSet = shared_descriptor_set,
          .dstBinding = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
          .pBufferInfo = &buffer_info},
      // shared_descriptor_set binding 1 is written in
      // crow::create_swapchain_callback()

      VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = top_as->get_descriptor_info(),
          .dstSet = raytracing_descriptor_set,
          .dstBinding = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR},

      VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .dstSet = raytracing_descriptor_set,
          .dstBinding = 1,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
          .pBufferInfo = instance_buffer->get_descriptor_info()},

      VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                           .dstSet = raytracing_descriptor_set,
                           .dstBinding = 2,
                           .descriptorCount = 1,
                           .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                           .pBufferInfo = vertex_buffer->get_descriptor_info()},

      VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                           .dstSet = raytracing_descriptor_set,
                           .dstBinding = 3,
                           .descriptorCount = 1,
                           .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                           .pBufferInfo = index_buffer->get_descriptor_info()}};

  for (auto const& write_set : write_sets) {
    writes_stack.push(write_set);
  }
}

auto create_raytracing_image() -> lava::image::ptr {
  // Output image for the raytracing shader.
  // RGBA 16 is guaranteed to support these usage flags.
  VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT;
  auto output_image = lava::make_image(format);
  output_image->set_usage(
      VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
  output_image->set_layout(VK_IMAGE_LAYOUT_UNDEFINED);
  output_image->set_aspect_mask(lava::format_aspect_mask(format));
  return output_image;
}

}  // namespace crow
