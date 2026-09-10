# libmoltenvk - Vulkan implementation on top of Apple's Metal

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
C++ library. It implements a subset of the Vulkan 1.4 graphics and compute API
on top of Apple's Metal framework. This package currently targets macOS only.


## Usage

This package builds MoltenVK, Apple's Vulkan-over-Metal ICD. Three ways to
consume it, each with its own working, automated test in the accompanying
`libmoltenvk-tests` package (`direct-driver-loading/`, `icd/`, `bundle/`) or
in this package's own `tests/basics/` (direct-link). Each needs real Metal
to run the actual smoke test, so it first probes for hardware and falls
back to checking just that diagnostic on a host without it (e.g. CI).
Do not mix ICD mode or direct driver loading with direct-link mode in the
same image: `lib{MoltenVK}` and the Khronos loader both export `vk*`.

### Direct driver loading mode (recommended)

> Build this package's MoltenVK alongside the application, without linking
> it, and hand it to the loader directly at runtime via
> `VK_LUNARG_direct_driver_loading`. Guarantees the application runs against
> this exact, tested build: no ICD json, no environment variable, no
> filesystem search that could instead pick up whatever (or whichever
> version of) MoltenVK happens to already be registered on the machine.

```
depends: libmoltenvk ^1.4.2 ? ($cxx.target.class == 'macos')
```

```
import  vk    = libvulkan-loader%lib{vulkan}
import  util  = libmoltenvk%lib{mvk-direct-util}
import! [metadata, rule_hint=cxx.link] dylib = libmoltenvk%libs{MoltenVK}

exe{app}: cxx{main} $vk $util
exe{app}: $dylib: include = adhoc

cxx.poptions += "-DMVK_DYLIB_PATH=\"$posix_string($($dylib: libmoltenvk.dylib_path))\""
```

`libs{MoltenVK}` is imported with the metadata protocol (`import!
[metadata, rule_hint=cxx.link]`), never linked, so its `vk*` symbols
never enter the image (the extension exists specifically for drivers
like MoltenVK that also implement the full Vulkan API surface, where a
normal link would collide with the loader's own symbols).
`lib{mvk-direct-util}` is the recommended helper for this mode: it
`dlopen`s the dylib with `RTLD_LOCAL` (keeping its symbols out of the
process-wide namespace too), resolves `vk_icdGetInstanceProcAddr` via
`dlsym`, and fills a `VkDirectDriverLoadingListLUNARG` for
`VkInstanceCreateInfo`'s `pNext` chain. It does not link `lib{MoltenVK}`.

```cpp
#include <mvk-direct-util/mvk-direct-util.hpp>

mvk_direct_util::direct_driver mvk;
if (!mvk_direct_util::open (mvk, MVK_DYLIB_PATH))
  abort ();

// VkInstanceCreateInfo.pNext = &mvk.list;
```

`open()` always runs `resolve_dylib_path` first. `MVK_DYLIB_PATH`
(`libmoltenvk.dylib_path`) is absolute when libmoltenvk is built from
source (a no-op) and relative to this executable once libmoltenvk is a
separately installed (`sys:`) dependency. `dlopen` of a relative path
is against cwd, not the binary, so the helper completes it against the
executable location.

Keep `mvk` alive across `vkCreateInstance` (`list.pDrivers` points at
`mvk.info`).

See `libmoltenvk-tests/direct-driver-loading/` for a complete, working
example covering both paths.

This extension is designed by LunarG specifically for API translation
layers like MoltenVK. It requires ICD interface version 7, which this
package's `vulkan.mm` is patched to report (see
`src/MoltenVK/Vulkan/vulkan.mm.patch`,
[KhronosGroup/MoltenVK#2663](https://github.com/KhronosGroup/MoltenVK/issues/2663)).

### ICD mode (conventional)

> Link only the Khronos loader. MoltenVK is discovered and `dlopen`ed as a
> runtime driver, exactly like any other Vulkan ICD (AMD, NVIDIA, etc). The
> conventional, zero-platform-code way Vulkan applications integrate a
> driver, but with no guarantee it is *this* package's build that gets
> loaded: search path and `VK_ADD_DRIVER_FILES` both pick up whatever ICD
> happens to already be registered on the machine.

```
depends: libvulkan-loader ^1.4.359
```

```
import vk = libvulkan-loader%lib{vulkan}
exe{app}: cxx{main} $vk
```

A library that only compiles against the loader must never depend on
`libmoltenvk` either, since that would pin one specific ICD onto every
application that links it.

At `vkCreateInstance`, enable `VK_KHR_portability_enumeration` and
`VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR` or the loader skips
this driver. That is Khronos policy, not specific to this package.

There is no `depends:` and no `import` in this mode, so nothing in a
consuming application's own `bdep sync`/`bpkg build` pulls this package in.
Building and installing *this* MoltenVK is therefore a separate, manual step
the consumer (or whoever administers the machine) runs once, ahead of time,
to a system default search path. A fresh, disposable `bpkg` configuration
(so this doesn't touch any configuration already in use) pointed at the
`stable` section of `cppget.org` (use `testing` instead if a required
dependency version isn't in `stable` yet):

```
bpkg create cxx config.cxx=clang++ -d /tmp/libmoltenvk-cfg
bpkg add    -d /tmp/libmoltenvk-cfg https://pkg.cppget.org/1/stable
bpkg fetch  -d /tmp/libmoltenvk-cfg
bpkg build  -d /tmp/libmoltenvk-cfg --for install libmoltenvk
bpkg install -d /tmp/libmoltenvk-cfg          \
             config.install.root=/usr/local   \
             config.install.sudo=sudo         \
             libmoltenvk
```

(`bpkg install` on its own only installs a package already configured by
`bpkg build`, hence the two separate steps: see
[`bpkg-pkg-install`](https://build2.org/bpkg/doc/bpkg-pkg-install.xhtml).)
After this the loader finds it exactly like any other installed ICD, no
application-side change at all.

How the loader finds this build of MoltenVK, using the same knobs a
non-`build2` user already has:

- **Search path.** Directories the loader always scans (no environment
  variable). On macOS that includes the app bundle
  `Contents/Resources/vulkan/icd.d/`, then `etc/vulkan/icd.d/` and
  `share/vulkan/icd.d/` under `/usr/local` and `/usr`, i.e. exactly the
  prefix installed above. See `libmoltenvk-tests/icd/`.
- **Environment.** For a prefix the loader doesn't scan, such as Homebrew
  (`/opt/homebrew`), a custom prefix, or an uninstalled out-tree, set
  `VK_ADD_DRIVER_FILES` (additive: adds to the driver list, unlike
  `VK_DRIVER_FILES`, which replaces it) to the json path. Same variable
  the LunarG SDK and a CMake MoltenVK build already use.
- **`.app` bundle.** Copy `json{MoltenVK_icd-bundle}` and
  `libMoltenVK.dylib` into `Contents/Resources/vulkan/icd.d/`. No
  environment variable, no search path configuration: a bundled ICD is
  found by the loader as soon as the app runs from inside the bundle.
  See `libmoltenvk-tests/bundle/`, which assembles a minimal `.app`
  structure (no `Info.plist` needed) and runs it with a completely
  empty environment to prove exactly this.

Installing to a search path does not make the loader prefer this build over
any other registered MoltenVK: every driver manifest found across every
scanned path is loaded and its devices enumerated, there is no single-winner
priority among them. If a different MoltenVK is already registered
(Homebrew, the LunarG SDK, another application's own bundle), both get
loaded side by side, a scenario the loader's own driver-discovery docs
single out by name as leading to potential issues or crashes ([Vulkan-Loader
`docs/LoaderDriverInterface.md`, "Driver Discovery on
macOS"](https://github.com/KhronosGroup/Vulkan-Loader/blob/main/docs/LoaderDriverInterface.md#driver-discovery-on-macos)).
`VK_DRIVER_FILES` (replaces the whole list, exclusive, used by
`libmoltenvk-tests/icd/`) is the only environment-based way to guarantee
just this build is considered. For a guarantee that does not depend on the
environment at all, see "Direct driver loading mode" above.

`library_path` in the json is relative to the json file. Relocatable
installation is supported.

GLFW should use the linked loader. Do not call `glfwInitVulkanLoader` in
ICD mode.

Out-of-tree testing of *this* repo (no system MoltenVK installed) is the
one case that legitimately needs the json path before install.
`libmoltenvk-tests/icd/` is the reference for how: a plain
same-configuration import of `json{MoltenVK_icd}` plus Testscript `env` to
scope `VK_DRIVER_FILES` to a single test invocation. That pattern is
dev/CI tooling for this repo, not something application packages should
copy.

### Direct-link mode

> Link `lib{MoltenVK}` as an ordinary C++ library, bypassing the loader
> entirely. For a consumer that only ever wants MoltenVK specifically,
> such as this package's own SPIR-V-to-MSL conversion tooling.

```
depends: libmoltenvk ^1.4.2
```

```
import libs = libmoltenvk%lib{MoltenVK}
```

The target name is mixed-case `lib{MoltenVK}` (not `lib{moltenvk}`) to match
the upstream binary name (`libMoltenVK.dylib`).

Linking against it transitively imports `libvulkan-headers` and the Apple
frameworks required by the public headers (`Foundation`, `Metal`,
`CoreGraphics`). GLFW then needs `glfwInitVulkanLoader(vkGetInstanceProcAddr)`
or it will `dlopen` `libvulkan.1.dylib`.

The ICD json is not used in this mode.


## Importable targets

This package provides the following importable targets:

```
lib{MoltenVK}
libs{MoltenVK}
lib{mvk-direct-util}
json{MoltenVK_icd}
json{MoltenVK_icd-bundle}
```

`lib{MoltenVK}` is the compiled library, for direct-link mode only.

`libs{MoltenVK}` is the shared library member specifically, for direct
driver loading mode: imported with `import! [metadata, rule_hint=cxx.link]`
and added as an adhoc prerequisite (never linked), it exports
`libmoltenvk.dylib_path` metadata giving the dylib's path for `dlopen`. See
"Direct driver loading mode" above for the complete example.

`lib{mvk-direct-util}` is the recommended helper for that same mode
(resolve the dylib path, `dlopen`, fill the loader `pNext` chain). It
is not a substitute for `lib{MoltenVK}` or the Khronos loader, and it
does not link `lib{MoltenVK}`.

`json{MoltenVK_icd}` is the out-of-tree ICD manifest. It exists for this
project's own out-of-tree testing (see `libmoltenvk-tests/icd/`) and is
not a general consumer API. Ordinary ICD-mode consumers need neither
`depends` nor `import` on this package at all.

`json{MoltenVK_icd-bundle}` is a static ICD manifest (`library_path`
relative to its own directory) for packagers copying MoltenVK into a
`.app` bundle's `Contents/Resources/vulkan/icd.d/`.


## Configuration variables

This package provides the following configuration variables:

```
[bool]   config.libmoltenvk.use_metal_private_api ?= false
[string] config.libmoltenvk.log_level             ?= 'info'
```

`config.libmoltenvk.use_metal_private_api` mirrors upstream's
`MVK_USE_METAL_PRIVATE_API`. When true, MoltenVK may use private Metal
interfaces to implement some Vulkan features. Default is false (App Store
safe).

`config.libmoltenvk.log_level` mirrors upstream's `MVK_CONFIG_LOG_LEVEL`.
Valid values are `debug`, `info`, `warn`, `error`, and `off`.
