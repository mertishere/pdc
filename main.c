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


void writePPM(
    Pixel *pixels,
    size_t pixel_size,
    PNG png
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

    char *gaussian = "out/gaussian.output.ppm";
    FILE *gaussian_file = fopen(gaussian, "wb");
    if (!gaussian_file) {
        printf("Opening gaussian_file.ppm failed.");
        return;
    }

    int width = png.ihdr.width;
    int height = png.ihdr.height;

    // P6 header
    fprintf(outline_black_file, "P6\n%d %d\n255\n", width, height);
    fprintf(fill_black_file, "P6\n%d %d\n255\n", width, height);
    fprintf(gaussian_file, "P6\n%d %d\n255\n", width, height);

    Pixel *gaussian_pixels = malloc(pixel_size * sizeof(Pixel));
    gaussianBlur(pixels, gaussian_pixels, png);

    Pixel *outline_pixels = malloc(pixel_size * sizeof(Pixel));
    outlineBlack(pixels, outline_pixels, png);

    Pixel *fill_pixels = malloc(pixel_size * sizeof(Pixel));
    fillBlack(pixels, fill_pixels, png);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // write fill file
            int index = y * width + x;
            Pixel fbp = fill_pixels[index];
            fputc(fbp.rgb.r, fill_black_file);
            fputc(fbp.rgb.g, fill_black_file);
            fputc(fbp.rgb.b, fill_black_file);


            // write outline file
            Pixel olp = outline_pixels[index];
            fputc(olp.rgb.r, outline_black_file);
            fputc(olp.rgb.g, outline_black_file);
            fputc(olp.rgb.b, outline_black_file);

            if(olp.rgb.r != RGB_BLACK || fbp.rgb.r != RGB_BLACK) {
                fputc(RGB_WHITE, gaussian_file);
                fputc(RGB_WHITE, gaussian_file);
                fputc(RGB_WHITE, gaussian_file);   
                continue;
            }

            // write gaussian file
            Pixel op = pixels[index];
            float op_total = 0;
            op_total += op.rgb.r;
            op_total += op.rgb.g;
            op_total += op.rgb.b;
            op_total /= 3;

            Pixel p = gaussian_pixels[index];
            float p_total = 0;
            p_total += p.rgb.r;
            p_total += p.rgb.g;
            p_total += p.rgb.b;
            p_total /= 3;

            int sad = abs((int)op_total - (int)p_total);

            fputc(sad, gaussian_file);
            fputc(sad, gaussian_file);
            fputc(sad, gaussian_file);
        }
    }

    free(gaussian_pixels);
    free(outline_pixels);
    free(fill_pixels);

    fclose(outline_black_file);
    fclose(fill_black_file);
    fclose(gaussian_file);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Path required.\n");
        return 1;
    }

    char *path = argv[1];
    size_t size = getFileSize(path);

    size_t buffer_size = size + BIT_LENGTH;
    char buffer[buffer_size];

    size_t hex_size = size * 2 + BIT_LENGTH;
    char hex[hex_size];
    
    size_t hex_len = getHexDump(
        buffer,
        buffer_size,

        hex,
        hex_size,
        path
    );

    int idats_amount = countIdats(hex, hex_len);
    IDAT *idats = malloc(idats_amount * sizeof(IDAT));
    if(idats == NULL) {
        printf("Allocation failed (idats).\n");
        exit(0);
    }

    int rgbs_amount = countRgbs(hex, hex_len);
    RGB *rgbs = malloc(rgbs_amount * sizeof(RGB));
    if(rgbs == NULL) {
        printf("Allocation failed (rgbs).\n");
        exit(0);
    }

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
    
    getPng(&png, hex, hex_len);
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

    size_t uncompressed_size = png.ihdr.width * png.ihdr.height * size_multiplicator;
    int *uncompressed = malloc(uncompressed_size * sizeof(int));
    if(uncompressed == NULL) {
        printf("Allocation failed (uncompressed).\n");
        exit(0);
    }

    size_t uncompressed_len = 0;
    getHuffman(
        binary,
        binary_len,
        uncompressed,
        &uncompressed_len,
        &huffman
    );
    
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

    writePPM(pixels, pixels_len, png);

    free(binary);
    free(uncompressed);
    free(pixels);
    for(int i = 0; i < idats_amount; i++) {
        free(png.idats[i].data);
    }

    free(idats);
    free(rgbs);
    return 0;
}
