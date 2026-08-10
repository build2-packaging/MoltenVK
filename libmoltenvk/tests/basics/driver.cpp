#include <cstdint>

#include <vulkan/vulkan.h>

#undef NDEBUG
#include <cassert>

// Minimal SPIR-V compute shader (GLSL equivalent):
//
//   #version 450
//   layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
//   void main() {}
//
static const uint32_t noop_compute_spirv[] =
{
  0x07230203, 0x00010000, 0x00000000, 0x00000005, 0x00000000, // header
  0x00020011, 0x00000001,                                     // OpCapability Shader
  0x0003000e, 0x00000000, 0x00000001,                         // OpMemoryModel Logical GLSL450
  0x0005000f, 0x00000005, 0x00000003, 0x6e69616d, 0x00000000, // OpEntryPoint GLCompute %main "main"
  0x00060010, 0x00000003, 0x00000011, 0x00000001, 0x00000001, // OpExecutionMode %main LocalSize
  0x00000001,                                                 //   1 1 1
  0x00020013, 0x00000001,                                     // %void    = OpTypeVoid
  0x00030021, 0x00000002, 0x00000001,                         // %voidfn  = OpTypeFunction %void
  0x00050036, 0x00000001, 0x00000003, 0x00000000, 0x00000002, // %main    = OpFunction %void None %voidfn
  0x000200f8, 0x00000004,                                     // %label   = OpLabel
  0x000100fd,                                                 // OpReturn
  0x00010038,                                                 // OpFunctionEnd
};

int main ()
{
  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo ici {};
  ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ici.pApplicationInfo = &app_info;

  VkInstance instance;
  assert (vkCreateInstance (&ici, nullptr, &instance) == VK_SUCCESS);

  uint32_t n (0);
  assert (vkEnumeratePhysicalDevices (instance, &n, nullptr) == VK_SUCCESS);
  assert (n > 0); // MoltenVK exposes at least one Metal-backed device.

  VkPhysicalDevice dev;
  n = 1;
  vkEnumeratePhysicalDevices (instance, &n, &dev);

  float prio (1.0f);
  VkDeviceQueueCreateInfo qci {};
  qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qci.queueFamilyIndex = 0;
  qci.queueCount = 1;
  qci.pQueuePriorities = &prio;

  VkDeviceCreateInfo dci {};
  dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  dci.queueCreateInfoCount = 1;
  dci.pQueueCreateInfos = &qci;

  VkDevice device;
  assert (vkCreateDevice (dev, &dci, nullptr, &device) == VK_SUCCESS);

  // Compute pipeline forces SPIR-V through MoltenVKShaderConverter / SPIRV-Cross.
  //
  VkShaderModuleCreateInfo smci {};
  smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  smci.codeSize = sizeof (noop_compute_spirv);
  smci.pCode = noop_compute_spirv;

  VkShaderModule shader;
  assert (vkCreateShaderModule (device, &smci, nullptr, &shader) == VK_SUCCESS);

  VkPipelineLayoutCreateInfo plci {};
  plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  VkPipelineLayout layout;
  assert (vkCreatePipelineLayout (device, &plci, nullptr, &layout) == VK_SUCCESS);

  VkComputePipelineCreateInfo cpci {};
  cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  cpci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  cpci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  cpci.stage.module = shader;
  cpci.stage.pName = "main";
  cpci.layout = layout;

  VkPipeline pipeline;
  assert (vkCreateComputePipelines (device, VK_NULL_HANDLE, 1, &cpci, nullptr, &pipeline) == VK_SUCCESS);

  vkDestroyPipeline (device, pipeline, nullptr);
  vkDestroyPipelineLayout (device, layout, nullptr);
  vkDestroyShaderModule (device, shader, nullptr);

  vkDestroyDevice (device, nullptr);
  vkDestroyInstance (instance, nullptr);
}
