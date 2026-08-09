# libmoltenvk - Vulkan implementation on top of Apple's Metal

This is a `build2` package for [MoltenVK](https://github.com/KhronosGroup/MoltenVK),
the open-source Khronos library that implements a subset of the Vulkan
graphics and compute API on top of Apple's Metal framework, on macOS.
`libmoltenvk` provides `libMoltenVK.dylib`, a complete Vulkan ICD (Installable
Client Driver): it can be linked directly by an application, or discovered at
runtime by `libvulkan-loader` via the ICD json installed to
`etc/vulkan/icd.d/`.

Note: this package targets macOS only and requires real Metal-capable
hardware to run anything built against it (see Testing below).


## Naming

The importable target is `lib{MoltenVK}` -- mixed case, deliberately not the
usual lowercase `lib{moltenvk}` convention. The ICD json's `library_path` and
the auto-generated `.pc` file's `Libs:` both need to resolve to exactly
`libMoltenVK.dylib`/`-lMoltenVK`, matching what the wider Vulkan/MoltenVK
ecosystem (docs, other packagers, `dlopen()` calls) universally expects.


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
Linking against it transitively imports `libvulkan-headers`, so consumers get
the Vulkan C headers (`<vulkan/vulkan.h>`) without a separate `depends:`
entry. `libspirv-cross` is an implementation-only dependency (MoltenVK's
shader converter uses it internally to translate SPIR-V to MSL) and is not
re-exported.


## Configuration variables

This package provides the following configuration variables:

```
[bool]   config.libmoltenvk.exclude_spirv_tools   ?= true
[bool]   config.libmoltenvk.exclude_cereal        ?= true
[bool]   config.libmoltenvk.use_metal_private_api ?= false
[string] config.libmoltenvk.log_level             ?= 'info'
```

`exclude_spirv_tools` and `exclude_cereal` mirror upstream's
`MVK_EXCLUDE_SPIRV_TOOLS`/`MVK_EXCLUDE_CEREAL` CMake options. Both currently
must stay `true`: turning either off would pull in `SPIRV-Tools`+
`SPIRV-Headers` or `cereal`, neither of which is packaged yet (the build
asserts with a clear message if you try). `use_metal_private_api` and
`log_level` (one of `debug`, `info`, `warn`, `error`, `off`) mirror the
matching CMake options directly.


## Known limitations

- This package's `depends:` on `libspirv-cross` requests a specific,
  non-cooperative component/namespace configuration
  (`glsl`/`msl`/`reflect` on, everything else off,
  `namespace=MVK_spirv_cross`), matching exactly what MoltenVK's own CMake
  build requests. If another package in the same build configuration depends
  on `libspirv-cross` with different values (especially a different
  `namespace`, which is ABI-affecting), the configurations cannot be
  reconciled and the build will fail to negotiate a dependency
  configuration. This is inherent to combining MoltenVK with another
  SPIRV-Cross consumer, not a bug in this package.
- The installed ICD json's `library_path` is an absolute, install-time-baked
  path (matching upstream's own CMake behavior) -- it is not relocatable.
  Re-installing to a different `config.install.root` regenerates it
  correctly, but copying an already-installed tree to a different location
  will not update it.
