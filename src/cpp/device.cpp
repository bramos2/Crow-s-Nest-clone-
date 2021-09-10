#include "../hpp/device.hpp"

#include <vector>

namespace crow {

auto create_logical_device(lava::device_manager& manager) -> lava::device::ptr {
  std::vector<char const*> extensions = {
      VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
      // next 3 required by VK_KHR_acceleration_structure
      VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
      VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
      // allow indexing using non-uniform values (ie. can diverge between shader
      // invocations)
      VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
      VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
      // required by VK_KHR_ray_tracing_pipeline
      VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
      // VK_KHR_RAY_QUERY_EXTENSION_NAME,
      // required by VK_KHR_ray_tracing_pipeline and VK_KHR_ray_query
      VK_KHR_SPIRV_1_4_EXTENSION_NAME,
      // required by VK_KHR_spirv_1_4
      VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
      // new layout for tightly-packed buffers (always uses alignment of base
      // type)
      VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
  };

  const VkPhysicalDeviceFeatures features = {
#ifdef DEBUG_RAYTRACING
      // bounds-check against buffer ranges
      .robustBufferAccess = VK_TRUE,
      // required for GPU-assisted validation
      // this needs to be enabled with vk_layer_settings.txt in the working
      // directory
      // can't check lava::config.debug.validation, because that gets
      // overwritten
      // in lava::app.setup() during debug builds,
      // but we need it earlier to create the device
      .vertexPipelineStoresAndAtomics = VK_TRUE,
      .fragmentStoresAndAtomics = VK_TRUE,
#endif
      // part of descriptorIndexing, see below
      .shaderSampledImageArrayDynamicIndexing = VK_TRUE,
      .shaderStorageBufferArrayDynamicIndexing = VK_TRUE,
  };

  VkPhysicalDeviceAccelerationStructureFeaturesKHR
      features_acceleration_structure = {
          .sType =
              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR,
          .accelerationStructure = VK_TRUE,
          .descriptorBindingAccelerationStructureUpdateAfterBind = VK_TRUE,
      };
  VkPhysicalDeviceBufferDeviceAddressFeaturesKHR features_buffer_device_address = {
      .sType =
          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR,
      .bufferDeviceAddress = VK_TRUE,
  };

  // VK_KHR_acceleration_structure requires the equivalent of the
  // descriptorIndexing feature
  // https://vulkan.lunarg.com/doc/view/1.2.162.0/windows/1.2-extensions/vkspec.html#features-descriptorIndexing
  VkPhysicalDeviceDescriptorIndexingFeaturesEXT features_descriptor_indexing = {
      .sType =
          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,
      // allow indexing into sampler arrays with non compile-time constants
      .shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE,
      .shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE,
      // allow indexing into sampler arrays with non uniform values
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
      .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
      .shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE,
      .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
      .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
      .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
      .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE,
      .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE,
      .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
      // allow only updating a subset of the max count in the layout
      .descriptorBindingPartiallyBound = VK_TRUE,
      // allow unbounded runtime descriptor arrays in shader (but fixed at
      // layout creation)
      .runtimeDescriptorArray = VK_TRUE,
  };

  VkPhysicalDeviceRayTracingPipelineFeaturesKHR features_ray_tracing_pipeline = {
      .sType =
          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR,
      .rayTracingPipeline = VK_TRUE,
      .rayTracingPipelineTraceRaysIndirect = VK_TRUE,
  };
  VkPhysicalDeviceScalarBlockLayoutFeaturesEXT features_scalar_block_layout = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
      .scalarBlockLayout = VK_TRUE,
  };

  features_acceleration_structure.pNext = &features_buffer_device_address;
  features_buffer_device_address.pNext = &features_descriptor_indexing;
  features_descriptor_indexing.pNext = &features_ray_tracing_pipeline;
  features_ray_tracing_pipeline.pNext = &features_scalar_block_layout;

  for (const lava::physical_device::ptr& physical_device :
       lava::instance::singleton().get_physical_devices()) {
    const VkPhysicalDeviceProperties& properties =
        physical_device->get_properties();
    if (properties.apiVersion < VK_API_VERSION_1_2) {
      continue;
    }

    lava::device::create_param device_params =
        physical_device->create_default_device_param();
    device_params.extensions.insert(device_params.extensions.end(),
                                    extensions.begin(), extensions.end());
    device_params.features = features;
    device_params.next = &features_acceleration_structure;
    lava::device::ptr device = manager.create(device_params);
    if (!device) {
      continue;
    }
    device->set_allocator(lava::create_allocator(
        device.get(), VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT));
    lava::log()->info("using device: {} ({})",
                      physical_device->get_properties().deviceName,
                      physical_device->get_device_type_string());
    return device;
  }
  // No device in the list was valid.
  lava::log()->error("no compatible device found");
  return nullptr;
}

}  // namespace crow
