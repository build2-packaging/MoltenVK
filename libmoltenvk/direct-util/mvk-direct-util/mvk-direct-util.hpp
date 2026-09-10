#pragma once

#include <string>

#include <vulkan/vulkan.h>

// Recommended helper for VK_LUNARG_direct_driver_loading. Never link
// lib{MoltenVK} (vk* collides with the Khronos loader). Pass
// libmoltenvk.dylib_path from libs{MoltenVK} metadata as dylib_path.
// See libmoltenvk/PACKAGE-README.md.
//
namespace mvk_direct_util
{
  std::string
  resolve_dylib_path (const char* path);

  // list.pDrivers points at info, so this type is not copyable or
  // movable. Keep it alive across vkCreateInstance.
  //
  struct direct_driver
  {
    direct_driver () = default;
    ~direct_driver ();
    direct_driver (const direct_driver&) = delete;
    direct_driver& operator= (const direct_driver&) = delete;
    direct_driver (direct_driver&&) = delete;
    direct_driver& operator= (direct_driver&&) = delete;

    void* handle = nullptr;
    VkDirectDriverLoadingInfoLUNARG info {};
    VkDirectDriverLoadingListLUNARG list {};
  };

  // resolve_dylib_path, dlopen (RTLD_NOW | RTLD_LOCAL), dlsym
  // vk_icdGetInstanceProcAddr, fill list as EXCLUSIVE. Returns false
  // on dlopen or dlsym failure.
  //
  bool
  open (direct_driver& d, const char* dylib_path);

  void
  close (direct_driver& d);
}
