# libmoltenvk - Vulkan implementation on top of Apple's Metal

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
C++ library. It implements a subset of the Vulkan 1.4 graphics and compute API
on top of Apple's Metal framework. This package currently targets macOS only.


## Usage

To start using `libmoltenvk` in your project, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: libmoltenvk ^1.4.2
```

Then import the library in your `buildfile`:

```
import libs = libmoltenvk%lib{MoltenVK}
```

The target name is mixed-case `lib{MoltenVK}` (not `lib{moltenvk}`) to match
the upstream binary name (`libMoltenVK.dylib`) expected by the Vulkan ICD
ecosystem.

Linking against it transitively imports `libvulkan-headers` and the Apple
frameworks required by the public headers (`Foundation`, `Metal`,
`CoreGraphics`).

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

SPIRV-Tools integration is always excluded in this package
(`MVK_EXCLUDE_SPIRV_TOOLS=1`). Enabling it requires SPIRV-Tools and
SPIRV-Headers packages that are not available yet.
