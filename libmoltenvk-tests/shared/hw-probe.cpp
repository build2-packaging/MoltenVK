#include "hw-probe.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

bool
probing_hardware (int argc, char* argv[])
{
  return argc > 1 && std::strcmp (argv[1], "--probe-hw") == 0;
}

void
print_hardware (VkInstance instance)
{
  uint32_t n (0);
  vkEnumeratePhysicalDevices (instance, &n, nullptr);

  std::vector<VkPhysicalDevice> devs (n);
  vkEnumeratePhysicalDevices (instance, &n, devs.data ());

  for (VkPhysicalDevice d: devs)
  {
    VkPhysicalDeviceProperties p;
    vkGetPhysicalDeviceProperties (d, &p);
    std::printf ("%s\n", p.deviceName);
  }
}
