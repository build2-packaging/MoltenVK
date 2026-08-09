# libmoltenvk - Vulkan implementation on top of Apple's Metal

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
C++ library. It implements a subset of the Vulkan graphics and compute API on
top of Apple's Metal framework, on macOS.


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


## Importable targets

This package provides the following importable targets:

```
lib{MoltenVK}
```

The shared/static library implementing the Vulkan ICD on top of Metal.
Linking against it transitively imports `libvulkan-headers`.


## Configuration variables

This package provides the following configuration variables:

```
[bool]   config.libmoltenvk.exclude_spirv_tools   ?= true
[bool]   config.libmoltenvk.exclude_cereal        ?= true
[bool]   config.libmoltenvk.use_metal_private_api ?= false
[string] config.libmoltenvk.log_level             ?= 'info'
```

Mirror upstream's `MVK_EXCLUDE_SPIRV_TOOLS`, `MVK_EXCLUDE_CEREAL`,
`MVK_USE_METAL_PRIVATE_API`, and `MVK_CONFIG_LOG_LEVEL` CMake options,
respectively.
