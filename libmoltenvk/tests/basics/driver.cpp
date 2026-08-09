#include <cstdint>

#include <vulkan/vulkan.h>

#undef NDEBUG
#include <cassert>

// Functional smoke test: create a Vulkan instance and device through
// MoltenVK's ICD entry points, linked directly (MoltenVK is a complete ICD
// and is routinely used this way, without going through libvulkan-loader).
// Requires real Metal-capable hardware.
//
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

  vkDestroyDevice (device, nullptr);
  vkDestroyInstance (instance, nullptr);
}
