#ifndef COMPUTE_H
#define COMPUTE_H

#include <stdbool.h>

#include "png.h"

typedef struct {
    int *points;
    size_t len;
    size_t capacity;
} Boundary;

typedef struct {
    Boundary *items;
    size_t len;
    size_t capacity;
} Boundaries;

extern void neighbourChecks(
    Pixel *pixels,
    size_t pixels_size,

    Boundaries *boundaries,
    PNG png
);

extern float computeBrightness(Pixel p);

extern void gaussianBlur(
    Pixel *pixels,
    size_t pixels_size,
    Pixel *output,
    size_t output_size,
    PNG png
);

extern void outlineBlack(
    Pixel *pixels,
    size_t pixels_size,
    Pixel *output,
    size_t output_size,
    PNG png
);

extern void fillBlack(
    Pixel *pixels,
    size_t pixels_size,
    Pixel *output,
    size_t output_size,
    PNG png
);

extern void sad(
    Pixel *blurred_pixels,
    size_t blurred_pixels_size,

    Pixel *original_pixels,
    size_t original_pixels_size,

    Pixel *output,
    size_t output_size,

    PNG png
);


#endif