#include "dylib-path.hpp"

#include <climits>
#include <filesystem>

#include <mach-o/dyld.h>

std::string
resolve_dylib_path (const char* path)
{
  namespace fs = std::filesystem;

  if (fs::path (path).is_absolute ())
    return path;

  char exe_path[PATH_MAX];
  uint32_t size = sizeof (exe_path);
  if (_NSGetExecutablePath (exe_path, &size) != 0)
    return path; // PATH_MAX is always big enough for _NSGetExecutablePath.

  return (fs::path (exe_path).parent_path () / path).lexically_normal ().string ();
}
