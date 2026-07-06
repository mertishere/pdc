#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "png.h"
#include "huffman.h"
#include "filter.h"
#include "utils.h"


void write_ppm(const char *filename, Pixel *pixels, int width, int height) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return;
    }

    // P6 header
    fprintf(file, "P6\n%d %d\n255\n", width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel *p = &pixels[y * width + x];
            fputc(p->rgb.r, file);
            fputc(p->rgb.g, file);
            fputc(p->rgb.b, file);
        }
    }

    fclose(file);
}

int main() {
    char *path = "tests/landscape.png";
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
