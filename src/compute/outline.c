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

float computeBrightness(Pixel p) {
    float brightness = R_FLOAT_COEFFICIENTS * (float)p.rgb.r;
    brightness += G_FLOAT_COEFFICIENTS * (float)p.rgb.g;
    brightness += B_FLOAT_COEFFICIENTS * (float)p.rgb.b;
    return brightness;
}

void outlineBlack(
    Pixel *pixels,
    size_t pixels_size,

    Pixel *output,
    size_t output_size,

    PNG png
) {
    int width = png.ihdr.width;
    int height = png.ihdr.height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t tlp_index = (y - (y != 0)) * width + (x - (x != 0));
            size_t tmp_index = (y - (y != 0)) * width + x;
            size_t trp_index = (y - (y != 0)) * width + (x + (x != (width - 1)));

            size_t lp_index = y * width + (x - (x != 0));
            size_t rp_index = y * width + (x + (x != (width - 1)));
            size_t mp_index = y * width + x;

            size_t blp_index = (y + (y != (height - 1))) * width + (x - (x != 0));
            size_t bmp_index = (y + (y != (height - 1))) * width + x;
            size_t brp_index = (y + (y != (height - 1))) * width + (x + (x != (width - 1)));

            if(tlp_index > pixels_size) return;
            if(trp_index > pixels_size) return;
            if(tmp_index > pixels_size) return;
            if(lp_index > pixels_size) return;
            if(rp_index > pixels_size) return;
            if(mp_index > pixels_size) return;
            if(blp_index > pixels_size) return;
            if(brp_index > pixels_size) return;
            if(bmp_index > pixels_size) return;

            // top left, middle and right pixel
            Pixel tlp = pixels[tlp_index];
            Pixel tmp = pixels[tmp_index];
            Pixel trp = pixels[trp_index];

            // left, and right pixel
            Pixel lp = pixels[lp_index];
            Pixel rp = pixels[rp_index];

            // bottom left, middle and right pixel
            Pixel blp = pixels[blp_index];
            Pixel bmp = pixels[bmp_index];
            Pixel brp = pixels[brp_index];

            // middle pixel
            Pixel p = pixels[mp_index];

            float brightness = computeBrightness(p);
            brightness += computeBrightness(tlp);
            brightness += computeBrightness(tmp);
            brightness += computeBrightness(trp);
            brightness += computeBrightness(lp);
            brightness += computeBrightness(rp);
            brightness += computeBrightness(blp);
            brightness += computeBrightness(bmp);
            brightness += computeBrightness(brp);
            brightness /= 9;

            float center = computeBrightness(p);
            float diff = 0.0f;

            diff += fabsf(center - computeBrightness(tlp));
            diff += fabsf(center - computeBrightness(tmp));
            diff += fabsf(center - computeBrightness(trp));

            diff += fabsf(center - computeBrightness(lp));
            diff += fabsf(center - computeBrightness(rp));

            diff += fabsf(center - computeBrightness(blp));
            diff += fabsf(center - computeBrightness(bmp));
            diff += fabsf(center - computeBrightness(brp));
            diff /= 8.0f;

            if((size_t)(y * width + x) >= output_size) return;
            if (diff > 15.0f && brightness < 220.0f) {
                output[y * width + x].rgb.r = RGB_BLACK;
                output[y * width + x].rgb.g = RGB_BLACK;
                output[y * width + x].rgb.b = RGB_BLACK;
            } else {
                output[y * width + x].rgb.r = RGB_WHITE;
                output[y * width + x].rgb.g = RGB_WHITE;
                output[y * width + x].rgb.b = RGB_WHITE;
            }
            output[y * width + x].rgb.a = 255;
        }
    }

    // clean up
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel tmp = output[(y - (y != 0)) * width + x];
            Pixel lp = output[y * width + (x - (x != 0))];
            Pixel rp = output[y * width + (x + (x != (width - 1)))];
            Pixel bmp = output[(y + (y != (height - 1))) * width + x];

            Pixel p = output[y * width + x];

            if(p.rgb.r == RGB_WHITE) continue;
            
            int count =
                (tmp.rgb.r == RGB_BLACK) +
                (lp.rgb.r == RGB_BLACK) +
                (rp.rgb.r == RGB_BLACK) +
                (bmp.rgb.r == RGB_BLACK);

            if(count >= 2) continue;
            output[y * width + x].rgb.r = RGB_WHITE;
            output[y * width + x].rgb.g = RGB_WHITE;
            output[y * width + x].rgb.b = RGB_WHITE;
        }
    }
}
