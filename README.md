# MoltenVK - Vulkan implementation on top of Apple's Metal

This is a `build2` package repository for
[`MoltenVK`](https://github.com/KhronosGroup/MoltenVK), the open-source
Khronos library that implements a subset of the Vulkan graphics and compute
API on top of Apple's Metal framework, on macOS.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`MoltenVK` in your `build2`-based project, then instead see the accompanying
[`PACKAGE-README.md`](libmoltenvk/PACKAGE-README.md) file.

See [`PACKAGING.md`](PACKAGING.md) for the packaging design notes and
rationale (dependency graph tracing, the build-from-source vs.
prebuilt-XCFramework decision, source layout, compile-definition
reproduction, etc).

The development setup for `MoltenVK` uses the standard `bdep`-based workflow.
This package is macOS/Apple-Metal-only, so only an Apple-toolchain build
configuration makes sense (no Homebrew GCC/Clang variants -- see
`PACKAGING.md`). For example:

```
git clone .../MoltenVK.git
cd MoltenVK

bdep init -C ../MoltenVK-apple @apple -- cc config.cxx=/usr/bin/clang++
bdep update
bdep test
```
