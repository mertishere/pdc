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

void sad(
    Pixel *blurred_pixels,
    size_t blurred_size,

    Pixel *original_pixels,
    size_t original_size,

    Pixel *output,
    size_t output_size,

    PNG png
) {
    int width = png.ihdr.width;
    int height = png.ihdr.height;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            size_t index = y * width + x;
            if(index > blurred_size || index > original_size || index >= output_size) return;

            Pixel op = original_pixels[index];
            float op_total = 0;
            op_total += op.rgb.r;
            op_total += op.rgb.g;
            op_total += op.rgb.b;
            op_total /= 3;

            Pixel p = blurred_pixels[index];
            float p_total = 0;
            p_total += p.rgb.r;
            p_total += p.rgb.g;
            p_total += p.rgb.b;
            p_total /= 3;
            
            int sad = abs((int)op_total - (int)p_total);
            output[y * width + x].rgb.r = sad;
            output[y * width + x].rgb.g = sad;
            output[y * width + x].rgb.b = sad;
            output[y * width + x].rgb.a = 255;
        }
    }
}
