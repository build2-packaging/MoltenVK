#undef NDEBUG
#include <mvk-direct-util/mvk-direct-util.hpp>

#include <cassert>
#include <cstdio>

// Unit test for mvk_direct_util::resolve_dylib_path: no MoltenVK dylib
// involved, just the path arithmetic that ../direct-driver-loading and
// libmoltenvk/PACKAGE-README.md's "Direct driver loading mode"
// rely on. argv[1] is a path, relative to this executable's own
// directory, that ./testscript has staged a real file at beforehand.
//
int
main (int argc, char* argv[])
{
  assert (argc == 2);

  assert (mvk_direct_util::resolve_dylib_path ("/some/absolute/path") ==
          "/some/absolute/path");

  std::string resolved = mvk_direct_util::resolve_dylib_path (argv[1]);
  FILE* f = std::fopen (resolved.c_str (), "r");
  assert (f != nullptr);
  std::fclose (f);
}
