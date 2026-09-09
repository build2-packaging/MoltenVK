# libmoltenvk-tests - Vulkan implementation on top of Apple's Metal (tests)

This is a `build2` package containing the test suite for the
[`MoltenVK`](https://github.com/KhronosGroup/MoltenVK) library. Each
subdirectory is a separate `exe{driver}` exercising one way of getting a
`VkInstance` out of MoltenVK through the Khronos loader, sharing the
device-and-compute-pipeline exercise in `shared/exercise-device.hpp` and
the `--probe-hw` hardware detection in `shared/hw-probe.hpp` (`libue{}`
utility libraries, see `shared/buildfile`). This is dev/CI tooling for
testing an uninstalled `libmoltenvk` out of tree. See `libmoltenvk`'s
`PACKAGE-README.md` for how a real application should consume it. None
of these drivers are installed (`install = false`): they exist only for
`bpkg test`, not as tools or examples for a downstream consumer to run.

`icd/`: the ICD path. Link `libvulkan-loader%lib{vulkan}`, import
`libmoltenvk%libs{MoltenVK}`'s metadata for the ICD json path (and, for a
separately installed `libmoltenvk`, a `bin.rpath` entry so the loader's
own `dlopen` of the dylib can find its dependencies), set
`VK_DRIVER_FILES` for the duration of the test via Testscript `env`, and
create an instance with portability enumeration enabled.

`direct-driver-loading/`: `VK_LUNARG_direct_driver_loading`. Link
`libvulkan-loader%lib{vulkan}`, import `libmoltenvk%libs{MoltenVK}`'s
metadata for the dylib path (never linked), `dlopen` it directly at
runtime via `shared/dylib-path.{hpp,cpp}`'s `resolve_dylib_path`
(absolute when `libmoltenvk` is built from source, relative once it is a
separately installed dependency), and hand the loader
`vk_icdGetInstanceProcAddr` through `VkInstanceCreateInfo`'s `pNext`
chain. No json, no environment variable. Requires ICD interface version
7, which a patched `libmoltenvk/src/MoltenVK/Vulkan/vulkan.mm` reports,
see [KhronosGroup/MoltenVK#2663](https://github.com/KhronosGroup/MoltenVK/issues/2663).


## Importable targets

This package exports no targets.


## Configuration variables

This package provides no configuration variables.
