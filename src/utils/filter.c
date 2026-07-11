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

float computeBrightness(Pixel p) {
    float brightness = R_FLOAT_COEFFICIENTS * (float)p.rgb.r;
    brightness += G_FLOAT_COEFFICIENTS * (float)p.rgb.g;
    brightness += B_FLOAT_COEFFICIENTS * (float)p.rgb.b;
    return brightness;
}

void outlineBlack(
    Pixel *pixels,
    Pixel *output,
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

            if (diff > 15.0f && brightness < 200.0f) {
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

void fillBlack(
    Pixel *pixels,
    Pixel *output,
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

void gaussianBlur(
    Pixel *pixels,
    Pixel *output,
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
                    Pixel pixel = pixels[(y + ky) * width + (x + kx)];
                    float brightness = computeBrightness(pixel);
                    sum += brightness * kernel[ky + 1][kx + 1];
                }
            }

            int pixel = (int)(sum + 0.5);
            output[y * width + x].rgb.r = pixel;
            output[y * width + x].rgb.g = pixel;
            output[y * width + x].rgb.b = pixel;
            output[y * width + x].rgb.a = 255;
        }
    }
}
