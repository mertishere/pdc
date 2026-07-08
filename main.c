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
#include "utils.h"
#include "constants.h"

float computeBrightness(Pixel *p) {
    float brightness = R_FLOAT_COEFFICIENTS * (float)p->rgb.r;
    brightness += G_FLOAT_COEFFICIENTS * (float)p->rgb.g;
    brightness += B_FLOAT_COEFFICIENTS * (float)p->rgb.b;
    return brightness;
}

void write_ppm(
    const char *filename,
    Pixel *pixels,
    int width,
    int height
) {
    DIR *dp = opendir("out");
    if(dp == NULL) {
        int d = mkdir("out", 0755); 
        if(d < 0) {
            printf("Making out/ directory failed.");
            exit(0);
        }
    } else {
        closedir(dp);
    }

    char *outline_black = "out/outline.output.ppm";
    FILE *outline_black_file = fopen(outline_black, "wb");
    if (!outline_black_file) {
        printf("Opening outline_black_file.ppm failed.");
        return;
    }

    char *fill_black = "out/black.output.ppm";
    FILE *fill_black_file = fopen(fill_black, "wb");
    if (!fill_black_file) {
        printf("Opening fill_black_file.ppm failed.");
        return;
    }

    // P6 header
    fprintf(outline_black_file, "P6\n%d %d\n255\n", width, height);
    fprintf(fill_black_file, "P6\n%d %d\n255\n", width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // top left, middle and right pixel
            Pixel *tlp = &pixels[(y - (y == 0 ? 0 : 1)) * width + (x - (x == 0 ? 0 : 1))];
            Pixel *tmp = &pixels[(y - (y == 0 ? 0 : 1)) * width + x];
            Pixel *trp = &pixels[(y - (y == 0 ? 0 : 1)) * width + (x + (x == (width - 1) ? 0 : 1))];

            // left, and right pixel
            Pixel *lp = &pixels[y * width + (x - (x == 0 ? 0 : 1))];
            Pixel *rp = &pixels[y * width + (x + (x == (width - 1) ? 0 : 1))];

            // bottom left, middle and right pixel
            Pixel *blp = &pixels[(y + (y == (height - 1) ? 0 : 1)) * width + (x - (x == 0 ? 0 : 1))];
            Pixel *bmp = &pixels[(y + (y == (height - 1) ? 0 : 1)) * width + x];
            Pixel *brp = &pixels[(y + (y == (height - 1) ? 0 : 1)) * width + (x + (x == (width - 1) ? 0 : 1))];

            // middle pixel
            Pixel *p = &pixels[y * width + x];

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

            // compute average
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
                fputc(RGB_BLACK, outline_black_file);
                fputc(RGB_BLACK, outline_black_file);
                fputc(RGB_BLACK, outline_black_file);
            } else {
                fputc(RGB_WHITE, outline_black_file);
                fputc(RGB_WHITE, outline_black_file);
                fputc(RGB_WHITE, outline_black_file);
            }

            if (brightness < 160.0f) {
                fputc(RGB_BLACK, fill_black_file);
                fputc(RGB_BLACK, fill_black_file);
                fputc(RGB_BLACK, fill_black_file);
            } else {
                fputc(RGB_WHITE, fill_black_file);
                fputc(RGB_WHITE, fill_black_file);
                fputc(RGB_WHITE, fill_black_file);
            }
        }
    }

    fclose(outline_black_file);
    fclose(fill_black_file);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Path required.\n");
        return 1;
    }

    char *path = argv[1];
    size_t size = getFileSize(path);

    size_t buffer_size = size * 2;
    char buffer[buffer_size];
    
    size_t hex_size = size;
    char hex[hex_size];
    
    getHexDump(
        buffer, 
        buffer_size,
        
        hex, 
        hex_size, 
        path
    );

    int idats_amount = countIdats(hex);

    IDAT idats[idats_amount];
    RGB rgbs[10];
    PNG png = {
        .idats = idats,
        .iend = {
            .size = 0
        },
        .ihdr = {
            .bits_per_pixel = 0,
            .color_type = 0,
            .compression_method = 0,
            .filter_method = 0,
            .height = 0,
            .interlaced = 0,
            .size = 0,
            .width = 0,
        },
        .plte = {
            .rgbs = rgbs,
            .size = 0,
        }
    };
    
    getPng(&png, hex);
    size_t idats_size = 0;
    for(int i = 0; i < idats_amount; i++) {
        size_t idat_data_size = png.idats[i].size;
        idats_size += idat_data_size;
    }
    
    size_t binary_len = 0;
    char *binary = malloc(idats_size * 8 * sizeof(char) - (4 * 4)); // - 4 * 4 (because of the first IDAT)
    if(binary == NULL) {
        printf("Allocation failed (binary).\n");
        exit(0);
    }

    for(int i = 0; i < idats_amount; i++) {
        size_t idat_data_size = png.idats[i].size * 2;
        if(i == 0) idat_data_size -= 4;
        char *idat_data = malloc(idat_data_size * 4 * sizeof(char));
        if(idat_data == NULL) {
            printf("Allocation failed (idat).\n");
            exit(0);
        }
        hexToBinary(png.idats[i].data, idat_data, idat_data_size, true);
        
        writeBufferOffset(
            idat_data,
            binary,
            
            0,
            idat_data_size * 4,
            
            binary_len // offset
        );
        binary_len += idat_data_size * 4;
        
        free(idat_data);
    }

    Huffman huffman = {
        .bfinal = 0,
        .btype = 0,
        .hclen = 0,
        .hdist = 0,
        .hlit = 0
    };

    int size_multiplicator = 0;
    switch (png.ihdr.color_type) {
        case 0:
            size_multiplicator = 1;
            break;
            
        case 2:
            size_multiplicator = 3;
            break;
        
        case 4:
            size_multiplicator = 2;
            break;
        
        case 6:
            size_multiplicator = 4;
            break;
        
        default:
            break;
    }

    int *uncompressed = malloc(png.ihdr.width * png.ihdr.height * size_multiplicator * sizeof(int));
    if(uncompressed == NULL) {
        printf("Allocation failed (uncompressed).\n");
        exit(0);
    }

    size_t uncompressed_len = 0;
    getHuffman(binary, binary_len, uncompressed, &uncompressed_len, &huffman);
    
    size_t pixels_size = png.ihdr.height * (1 + png.ihdr.width * size_multiplicator);

    Pixel *pixels = malloc(pixels_size * sizeof(Pixel));
    if(pixels == NULL) {
        printf("Allocation failed (pixels).\n");
        exit(0);
    }

    size_t pixels_len = 0;
    getFilter(
        pixels,
        &pixels_len,

        uncompressed, 
        uncompressed_len,
        &png
    );

    write_ppm("output.ppm", pixels, png.ihdr.width, png.ihdr.height);

    free(binary);
    free(uncompressed);
    free(pixels);
    for(int i = 0; i < idats_amount; i++) {
        free(png.idats[i].data);
    }
    return 0;
}
