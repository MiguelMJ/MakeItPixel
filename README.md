

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

Here's an example of different results processing an image with different configurations
<p align="center">
<img width="49%" src="examples/monalisa.png">
</p>

|  | |
|---|---|
| <img src="examples/config1.png"> | <img src="examples/config2.png"> |
| <img src="examples/config3.png"> | <img src="examples/config4.png"> |

## Features

- Intelligent handling of color and palettes.
- Strategies for:
  - Color selection during scaling.
  - Color quantization after scaling.
  - Dithering.
- Fully customizable both by configuration file and console.

## Dependencies

Runtime libraries:

- [SFML v2.5.1](https://www.sfml-dev.org/index.php) 

## Build

If you have the dependencies, a C++ compiler and GNU Makefile in your system, you can simply run:

```shell
g++ src/* -Iinclude -lsfml-graphics -o makeitpixel
```

## License

Make It Pixel uses the MIT License, a copy of which you can find [here](LICENSE), in the repo.

The external library SFML is licensed under the zlib/png license.

The optional, external library GNU Readline is licensed under the GPLv3 license.
