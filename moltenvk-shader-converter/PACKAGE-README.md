# moltenvk-shader-converter - SPIR-V to Metal Shading Language shader converter command-line tool

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
project's stand-alone `MoltenVKShaderConverter` command-line tool. It converts
SPIR-V shader code to Metal Shading Language (MSL) source code, for use at
development time from the command line.


## Usage

To start using `moltenvk-shader-converter` in your project, add the following
build-time `depends` value to your `manifest`, adjusting the version
constraint as appropriate:

```
depends: * moltenvk-shader-converter ^1.4.2
```

Then import the executable in your `buildfile`:

```
import! mvksc = moltenvk-shader-converter%exe{MoltenVKShaderConverter}
```

It can also be installed and run directly from the command line:

```
MoltenVKShaderConverter -si input.spv -mo output.metal
```


## Importable targets

This package provides the following importable targets:

```
exe{MoltenVKShaderConverter}
```

The `MoltenVKShaderConverter` command-line tool.


## Configuration variables

This package provides no configuration variables.
