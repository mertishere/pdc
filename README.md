# PDC

A PNG processing library written in C.

PDC is an experimental project that aims to transform PNG images into 3D geometry. The project begins by implementing the complete PNG decoding pipeline from scratch, providing full control over image parsing, decompression, and pixel reconstruction before moving on to image analysis and mesh generation.

The current implementation focuses on correctness, simplicity, and understanding the PNG format rather than performance or feature completeness.

## Usage

Run;

```sh
git clone https://github.com/mertishere/pdc

cd pdc

make
```

To use PDC, simply pass the path as the first argument.
```sh
./dist/pdc landscape.png
```

## Features

Current capabilities:

- PNG signature validation
- Critical chunk parsing
- DEFLATE decompression
- Scanline reconstruction
- PPM image output for verification

The generated image can be opened with:

```sh
xdg-open output.ppm
```

## Vision

PDC is intended to become a complete pipeline for converting PNG images into 3D models.

Planned stages include:

```
PNG
 └── Parsing
      └── Decompression
           └── Pixel Reconstruction
                └── Image Analysis
                     └── Geometry Generation
                          └── Mesh Export
```

The long-term goal is to produce 3D meshes directly from image data without relying on external image processing libraries.

## Roadmap

- [ ] Parse ancillary PNG chunks
- [ ] Support additional PNG color types
- [ ] Improve memory efficiency
- [ ] Improve allocation strategy
- [ ] Strengthen error handling
- [ ] Image feature extraction
- [ ] Height map generation
- [ ] Mesh generation
- [ ] 3D model export

## Project Status

PDC is experimental.

The decoder is intentionally implemented from first principles. Correctness and understanding of the PNG format take priority over optimization while the core architecture is being developed.

## Philosophy

PDC follows the cherries.works philosophy:

- Build from first principles.
- Prefer understanding over abstraction.
- Keep dependencies to a minimum.
- Design software that is simple to inspect, modify, and extend.
