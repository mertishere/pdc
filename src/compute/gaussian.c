#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "png.h"
#include "huffman.h"
#include "filter.h"
#include "constants.h"
#include "utils.h"
#include "compute.h"

void gaussianBlur(
    Pixel *pixels,
    size_t pixels_size,

    Pixel *output,
    size_t output_size,

    PNG png
) {
    // 3x3 gaussian kernel
    float kernel[3][3] = {
        {1.0/16, 2.0/16, 1.0/16},
        {2.0/16, 4.0/16, 2.0/16},
        {1.0/16, 2.0/16, 1.0/16}
    };

    int width = png.ihdr.width;
    int height = png.ihdr.height;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0.0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    size_t index = (y + ky) * width + (x + kx);
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
}
