

<p align="center"><img height="64px" src="doc/logo.png"></p>

***

<h3 align="center">Make images look like pixel art</h3>

<p align="center"><img src="https://img.shields.io/badge/C++-11-00599C?style=flat-square&logo=c%2B%2B"> <img src="https://img.shields.io/badge/SFML-v2.5.1-8CC445?logo=SFML&style=flat-square"> <a href="https://miguelmj.github.io/Candle"><!-- img src="https://img.shields.io/badge/code-documented-success?style=flat-square"/ --></a> <img src="https://img.shields.io/badge/version-v0.0_alpha-yellow?style=flat-square"/> <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-informational?style=flat-square"/></a></p>

Make It Pixel is a programming language to process images to look like pixel art. Its interpreter is written in C++ and uses SFML as its graphics library.

- [Preview](#preview)
- [Features](#features)
- [Dependencies](#dependencies)
- [Build](#build)
- [License](#license)

## Preview

(image and output examples here)

## Features

- Intelligent, native handling of color and palettes.
- Customizable strategies for:
  - Pixelization.
  - Color quantization.
  - Dithering.
- Support for script mode and interactive mode.
- Live preview for the processed images in the interactive mode.

## Dependencies

Runtime libraries:

- [SFML v2.5.1](https://www.sfml-dev.org/index.php) (required)
- [GNU Readline 8.1](https://tiswww.case.edu/php/chet/readline/rltop.html) (optional for the interactive mode)

## Build

If you have the dependencies, a C++ compiler and GNU Makefile in your system, you can simply run:

```shell
make
```

To build the documentation, [Doxygen 1.9.1](https://www.doxygen.nl/index.html) is required.

```shell
make docs
```

## License

Make It Pixel uses the MIT License, a copy of which you can find [here](LICENSE), in the repo.

The external library SFML is licensed under the zlib/png license.

The optional, external library GNU Readline is licensed under the GPLv3 license.
