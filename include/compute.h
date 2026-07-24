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
    PNG *png,
    Pixel *outline_pixels,
    Boundaries *boundaries
);

extern float computeBrightness(Pixel p);

extern void gaussianBlur(
    Pixel *output,
    size_t output_size,
    PNG *png,

    int radius
);

extern void outlineBlack(
    Pixel *output,
    size_t output_size,
    PNG *png
);

extern void fillBlack(
    Pixel *output,
    size_t output_size,
    PNG *png
);

extern void sad(
    Pixel *blurred_pixels,
    size_t blurred_pixels_size,

    Pixel *output,
    size_t output_size,

    PNG *png
);


#endif