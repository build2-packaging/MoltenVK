#undef NDEBUG
#include "dylib-path.hpp"

#include <cassert>
#include <cstdio>

// Unit test for resolve_dylib_path (see ../shared/dylib-path.cpp): no
// Vulkan/MoltenVK involved, just the path arithmetic that
// ../direct-driver-loading and ../../libmoltenvk/PACKAGE-README.md's
// "Direct driver loading mode" rely on. argv[1] is a path, relative to
// this executable's own directory, that ./testscript has staged a
// real file at beforehand.
//
int
main (int argc, char* argv[])
{
  assert (argc == 2);

  assert (resolve_dylib_path ("/some/absolute/path") == "/some/absolute/path");

  std::string resolved = resolve_dylib_path (argv[1]);
  FILE* f = std::fopen (resolved.c_str (), "r");
  assert (f != nullptr);
  std::fclose (f);
}
