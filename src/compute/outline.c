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

float computeBrightness(Pixel p) {
    float brightness = R_FLOAT_COEFFICIENTS * (float)p.rgb.r;
    brightness += G_FLOAT_COEFFICIENTS * (float)p.rgb.g;
    brightness += B_FLOAT_COEFFICIENTS * (float)p.rgb.b;
    return brightness;
}

void outlineBlack(
    Pixel *output,
    size_t output_size,
    PNG *png
) {
    const int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, 
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (size_t y = 0; y < png->ihdr.height; y++) {
        for (size_t x = 0; x < png->ihdr.width; x++) {
            float center = computeBrightness(png->pixels.data[y * png->ihdr.width + x]);
            float brightness_sum = center;
            float diff_sum = 0.0f;

            for (int i = 0; i < 8; i++) {
                size_t ny = y + offsets[i][0];
                size_t nx = x + offsets[i][1];

                if (ny >= png->ihdr.height || nx >= png->ihdr.width) continue;

                size_t n_index = ny * png->ihdr.width + nx;
                if(n_index > png->pixels.size) continue;
                Pixel neighbor = png->pixels.data[n_index];
                float neighbor_brightness = computeBrightness(neighbor);
                brightness_sum += neighbor_brightness;
                diff_sum += fabsf(center - neighbor_brightness);
            }

            float brightness = brightness_sum / 9.0f;
            float diff = diff_sum / 8.0f;

            // Apply threshold
            size_t oi = y * png->ihdr.width + x;
            if (oi >= output_size) continue;
            if (diff > 15.0f && brightness < 220.0f) {
                output[oi].rgb.r = RGB_BLACK;
                output[oi].rgb.g = RGB_BLACK;
                output[oi].rgb.b = RGB_BLACK;
            } else {
                output[oi].rgb.r = RGB_WHITE;
                output[oi].rgb.g = RGB_WHITE;
                output[oi].rgb.b = RGB_WHITE;
            }
            output[oi].rgb.a = 255;
        }
    }

    // clean up
    for (size_t y = 0; y < png->ihdr.height; y++) {
        for (size_t x = 0; x < png->ihdr.width; x++) {
            Pixel tmp = output[(y - (y != 0)) * png->ihdr.width + x];
            Pixel lp = output[y * png->ihdr.width + (x - (x != 0))];
            Pixel rp = output[y * png->ihdr.width + (x + (x != (png->ihdr.width - 1)))];
            Pixel bmp = output[(y + (y != (png->ihdr.height - 1))) * png->ihdr.width + x];

            Pixel p = output[y * png->ihdr.width + x];

            if(p.rgb.r == RGB_WHITE) continue;
            
            int count =
                (tmp.rgb.r == RGB_BLACK) +
                (lp.rgb.r == RGB_BLACK) +
                (rp.rgb.r == RGB_BLACK) +
                (bmp.rgb.r == RGB_BLACK);

            if(count >= 2) continue;
            output[y * png->ihdr.width + x].rgb.r = RGB_WHITE;
            output[y * png->ihdr.width + x].rgb.g = RGB_WHITE;
            output[y * png->ihdr.width + x].rgb.b = RGB_WHITE;
        }
    }
}
