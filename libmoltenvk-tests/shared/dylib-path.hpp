#pragma once

#include <string>

// Resolves libmoltenvk.dylib_path (or any path with the same
// before/after install contract - see libmoltenvk/PACKAGE-README.md's
// "Direct driver loading mode") into a path dlopen can use directly.
// Absolute paths (development, out-of-tree builds) are returned
// unchanged. Relative paths (installed, config.install.relocatable=true
// builds) are resolved against this process's own executable
// directory: dlopen resolves a relative path against the current
// working directory, not the binary, and an installed app can be
// launched from any cwd.
//
std::string
resolve_dylib_path (const char* path);
