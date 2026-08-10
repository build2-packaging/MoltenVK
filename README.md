# MoltenVK - Vulkan implementation on top of Apple's Metal

This is a `build2` package repository for [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK),
a layered implementation of Vulkan graphics and compute functionality built on
Apple's Metal framework on macOS.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`MoltenVK` in your `build2`-based project, then instead see the accompanying
`PACKAGE-README.md` files:

* [`libmoltenvk/PACKAGE-README.md`](libmoltenvk/PACKAGE-README.md)
* [`moltenvk-shader-converter/PACKAGE-README.md`](moltenvk-shader-converter/PACKAGE-README.md)

The development setup for `MoltenVK` uses the standard `bdep`-based workflow.
For example:

```
git clone .../MoltenVK.git
cd MoltenVK

bdep init -C @apple cc config.cxx=/usr/bin/clang++
bdep update
bdep test
```
