# moltenvk-shader-converter - SPIR-V to Metal Shading Language shader converter command-line tool

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
project's stand-alone `MoltenVKShaderConverter` command-line tool. It converts
SPIR-V shader code to Metal Shading Language (MSL) source code, for use at
development time from the command line.


## Usage

Install the package and run the `MoltenVKShaderConverter` executable directly
from the command line:

```
MoltenVKShaderConverter -si input.spv -mo output.metal
```

This package is not intended to be imported into another `build2` package's
`buildfile`.


## Importable targets

This package provides the following importable targets:

```
exe{MoltenVKShaderConverter}
```

The `MoltenVKShaderConverter` command-line tool.


## Configuration variables

This package provides no configuration variables.
