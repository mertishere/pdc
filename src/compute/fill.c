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

void fillBlack(
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
            // top left, middle and right pixel
            Pixel tlp = pixels[(y - (y != 0)) * width + (x - (x != 0))];
            Pixel tmp = pixels[(y - (y != 0)) * width + x];
            Pixel trp = pixels[(y - (y != 0)) * width + (x + (x != (width - 1)))];

            // left, and right pixel
            Pixel lp = pixels[y * width + (x - (x != 0))];
            Pixel rp = pixels[y * width + (x + (x != (width - 1)))];

            // bottom left, middle and right pixel
            Pixel blp = pixels[(y + (y != (height - 1))) * width + (x - (x != 0))];
            Pixel bmp = pixels[(y + (y != (height - 1))) * width + x];
            Pixel brp = pixels[(y + (y != (height - 1))) * width + (x + (x != (width - 1)))];

            // middle pixel
            Pixel p = pixels[y * width + x];

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
