#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "png.h"
#include "algorithms.h"
#include "constants.h"
#include "utils.h"
#include "compute.h"

void generateGaussianKernel(
    float **kernel,
    int radius,
    float sigma
) {
    int size = radius * 2 + 1;
    float sum = 0.0f;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {

            float value = expf(
                -(x * x + y * y) / (2.0f * sigma * sigma)
            );

            kernel[y + radius][x + radius] = value;
            sum += value;
        }
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            kernel[y][x] /= sum;
        }
    }
}

void gaussianBlur(
    Pixel *pixels,
    size_t pixels_size,

    Pixel *output,
    size_t output_size,

    int radius,

    PNG png
) {
    int kernel_size = radius * 2 + 1;
    float **kernel = malloc(kernel_size * sizeof(float *));
    for (int i = 0; i < kernel_size; i++) {
        kernel[i] = malloc(kernel_size * sizeof(float));
    }
    generateGaussianKernel(
        kernel,
        radius,
        radius / 2.0f
    );

    int width = png.ihdr.width;
    int height = png.ihdr.height;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0.0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int px = x + kx;
                    int py = y + ky;

                    px = px < 0 ? 0 : px >= width ? width - 1 : px;
                    py = py < 0 ? 0 : py >= height ? height - 1 : py;

                    size_t index = py * width + px;
                    if(index >= pixels_size) return;

                    Pixel pixel = pixels[index];
                    float brightness = computeBrightness(pixel);
                    sum += brightness * kernel[ky + 1][kx + 1];
                }
            }

            size_t index = (y * width + x);
            if(index >= output_size) return;

            int pixel = (int)(sum + 0.5);
            output[index].rgb.r = pixel;
            output[index].rgb.g = pixel;
            output[index].rgb.b = pixel;
            output[index].rgb.a = 255;
        }
    }

    for(int i = 0; i < kernel_size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}
