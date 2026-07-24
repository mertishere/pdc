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

void fillBlack(
    Pixel *output,
    size_t output_size,

    PNG *png
) {
    int width = png->ihdr.width;
    int height = png->ihdr.height;

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

            if(tlp_index > png->pixels.size) return;
            if(trp_index > png->pixels.size) return;
            if(tmp_index > png->pixels.size) return;
            if(lp_index > png->pixels.size) return;
            if(rp_index > png->pixels.size) return;
            if(mp_index > png->pixels.size) return;
            if(blp_index > png->pixels.size) return;
            if(brp_index > png->pixels.size) return;
            if(bmp_index > png->pixels.size) return;

            // top left, middle and right pixel
            Pixel tlp = png->pixels.data[tlp_index];
            Pixel tmp = png->pixels.data[tmp_index];
            Pixel trp = png->pixels.data[trp_index];

            // left, and right pixel
            Pixel lp = png->pixels.data[lp_index];
            Pixel rp = png->pixels.data[rp_index];

            // bottom left, middle and right pixel
            Pixel blp = png->pixels.data[blp_index];
            Pixel bmp = png->pixels.data[bmp_index];
            Pixel brp = png->pixels.data[brp_index];

            // middle pixel
            Pixel p = png->pixels.data[mp_index];

            // compute average
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

            if((size_t)(y * width + x) >= output_size) return;
            if (brightness < 160.0f) {
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
}
