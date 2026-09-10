#include <mvk-direct-util/mvk-direct-util.hpp>

#include <climits>
#include <filesystem>

#include <dlfcn.h>
#include <mach-o/dyld.h>

namespace mvk_direct_util
{
  std::string
  resolve_dylib_path (const char* path)
  {
    if (std::filesystem::path (path).is_absolute ())
      return path;

    char exe[PATH_MAX];
    uint32_t n = sizeof (exe);
    if (_NSGetExecutablePath (exe, &n) != 0)
      return path;

    return (std::filesystem::path (exe).parent_path () / path)
      .lexically_normal ().string ();
  }

  typedef PFN_vkVoidFunction (VKAPI_PTR *pfn_vk_icdGetInstanceProcAddr) (
    VkInstance, const char*);

  bool
  open (direct_driver& d, const char* dylib_path)
  {
    close (d);

    d.handle = dlopen (resolve_dylib_path (dylib_path).c_str (),
                       RTLD_NOW | RTLD_LOCAL);
    if (d.handle == nullptr)
      return false;

    auto get_proc_addr = (pfn_vk_icdGetInstanceProcAddr)
      dlsym (d.handle, "vk_icdGetInstanceProcAddr");
    if (get_proc_addr == nullptr)
    {
      close (d);
      return false;
    }

    d.info = {};
    d.info.sType = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_INFO_LUNARG;
    d.info.pfnGetInstanceProcAddr =
      (PFN_vkGetInstanceProcAddrLUNARG) get_proc_addr;

    d.list = {};
    d.list.sType = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_LIST_LUNARG;
    d.list.mode = VK_DIRECT_DRIVER_LOADING_MODE_EXCLUSIVE_LUNARG;
    d.list.driverCount = 1;
    d.list.pDrivers = &d.info;
    return true;
  }

  void
  close (direct_driver& d)
  {
    if (d.handle != nullptr)
    {
      dlclose (d.handle);
      d.handle = nullptr;
    }
  }

  direct_driver::~direct_driver ()
  {
    close (*this);
  }
}
