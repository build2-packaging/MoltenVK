#undef NDEBUG
#include "exercise-device.hpp"
#include "hw-probe.hpp"

#include <mvk-direct-util/mvk-direct-util.hpp>

#include <cassert>
#include <cstdio>

// Direct driver loading (VK_LUNARG_direct_driver_loading): dlopen
// MoltenVK directly (RTLD_LOCAL, so its vk* symbols never enter the
// global namespace and never collide with the linked loader's), pull
// vk_icdGetInstanceProcAddr, and hand it to the loader via the
// VkInstanceCreateInfo pNext chain. No json, no environment variable.
// MVK_DYLIB_PATH is libmoltenvk.dylib_path from libs{MoltenVK} (see
// ./buildfile): absolute in a same-configuration (dev) build, relative
// to this executable's own location once libmoltenvk is installed (see
// mvk_direct_util::open and libmoltenvk/PACKAGE-README.md's
// "Direct driver loading mode"). MVK_TEST_DYLIB_PATH is a test-only
// fallback, see the open() calls below.
//
int
main (int argc, char* argv[])
{
  mvk_direct_util::direct_driver mvk;
  if (!mvk_direct_util::open (mvk, MVK_DYLIB_PATH))
  {
    // libmoltenvk.dylib_path.for_install is relative to this executable's
    // own install location, so it only resolves correctly when this
    // executable is installed alongside libmoltenvk under the same
    // prefix (see ../../libmoltenvk/src/buildfile). This test binary
    // never is (it only ever runs out of a build or test directory), so
    // when libmoltenvk is consumed via sys: (a separately installed
    // dependency, e.g. CI's test-installed phase) that always fails.
    // MVK_TEST_DYLIB_PATH (libmoltenvk.test_dylib_path, always absolute)
    // exists purely so this test can still dlopen the real dylib and
    // exercise direct driver loading itself in that case.
    //
    assert (mvk_direct_util::open (mvk, MVK_TEST_DYLIB_PATH));
  }

  const char* exts[] =
  {
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    VK_LUNARG_DIRECT_DRIVER_LOADING_EXTENSION_NAME,
  };

  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo ici {};
  ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ici.pNext = &mvk.list;
  ici.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  ici.pApplicationInfo = &app_info;
  ici.enabledExtensionCount = 2;
  ici.ppEnabledExtensionNames = exts;

  // Requires ICD interface version 7 (KhronosGroup/MoltenVK#2663): see
  // ../../libmoltenvk/src/MoltenVK/Vulkan/vulkan.mm.patch for the fix to
  // vk_icdNegotiateLoaderICDInterfaceVersion's negotiated version.
  //
  VkInstance instance;
  VkResult r = vkCreateInstance (&ici, nullptr, &instance);

  if (probing_hardware (argc, argv))
  {
    if (r != VK_SUCCESS)
    {
      std::printf ("no compatible Metal hardware found\n");
      return 1;
    }

    print_hardware (instance);
    vkDestroyInstance (instance, nullptr);
    return 0;
  }

  assert (r == VK_SUCCESS);

  exercise_device (instance);

  vkDestroyInstance (instance, nullptr);
}
