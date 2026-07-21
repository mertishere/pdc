#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "png.h"
#include "algorithms.h"
#include "utils.h"
#include "constants.h"
#include "compute.h"


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

    const char *normal = "out/normal.output.ppm";
    FILE *normal_file = fopen(normal, "wb");
    if (!normal_file) {
        printf("Opening normal.output.ppm failed.");
        return;
    }

    const char *neighbour = "out/neighbour.output.ppm";
    FILE *neighbour_file = fopen(neighbour, "wb");
    if (!neighbour_file) {
        printf("Opening neighbour.output.ppm failed.");
        return;
    }

    const char *gaussian = "out/gaussian.output.ppm";
    FILE *gaussian_file = fopen(gaussian, "wb");
    if (!gaussian_file) {
        printf("Opening gaussian.output.ppm failed.");
        return;
    }

    int width = png.ihdr.width;
    int height = png.ihdr.height;

    // P6 header
    fprintf(normal_file, "P6\n%d %d\n255\n", width, height);
    fprintf(neighbour_file, "P6\n%d %d\n255\n", width, height);
    fprintf(gaussian_file, "P6\n%d %d\n255\n", width, height);

    for(size_t i = 0; i < pixel_size; i++) {
        Pixel pixel = pixels[i];
        fputc(pixel.rgb.r, normal_file);
        fputc(pixel.rgb.g, normal_file);
        fputc(pixel.rgb.b, normal_file);
    }

    Pixel *outline_pixels = malloc(pixel_size * sizeof(Pixel));
    outlineBlack(
        pixels,
        pixel_size,

        outline_pixels,
        pixel_size,

        png
    );

    Boundaries boundaries = {0};
    boundaries.capacity = 16;
    boundaries.items = malloc(boundaries.capacity * sizeof(Boundary));
    neighbourChecks(
        outline_pixels,
        pixel_size,

        &boundaries,
        png
    );

    Pixel *neighbour_pixels = malloc(pixel_size * sizeof(Pixel));
    for(size_t i = 0; i < pixel_size; i++) {
        neighbour_pixels[i].rgb.r = RGB_WHITE;
        neighbour_pixels[i].rgb.g = RGB_WHITE;
        neighbour_pixels[i].rgb.b = RGB_WHITE;
    }

    for(size_t i = 0; i < boundaries.len; i++) {
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;

        for(size_t j = 0; j < boundaries.items[i].len; j++) {
            int p = boundaries.items[i].points[j];
            neighbour_pixels[p].rgb.r = r;
            neighbour_pixels[p].rgb.g = g;
            neighbour_pixels[p].rgb.b = b;
        }
    }

    Pixel *gaussian_pixels = malloc(pixel_size * sizeof(Pixel));
    gaussianBlur(
        pixels,
        pixel_size,
        gaussian_pixels,
        pixel_size,
        3,
        png
    );

    for(size_t i = 0; i < pixel_size; i++) {
        Pixel pixel = gaussian_pixels[i];
        fputc(pixel.rgb.r, gaussian_file);
        fputc(pixel.rgb.g, gaussian_file);
        fputc(pixel.rgb.b, gaussian_file);
    }

    int vertexes = 1;
    FILE *obj_file = fopen("out/output.obj", "w");
    for(size_t i = 0; i < boundaries.len; i++) {
        Boundary boundary = boundaries.items[i];
        size_t points_len = boundary.len;
        DPPoint points[points_len];

        for(size_t j = 0; j < points_len; j++) {
            int p = boundary.points[j];
            Pixel gp = gaussian_pixels[p];
            Pixel op = pixels[p];
            float f = computeBrightness(op);
            
            points[j].x = p % png.ihdr.width;
            points[j].y = p / png.ihdr.height;
            points[j].z = (f - gp.rgb.r) * 100.0f / 255.0f;
            points[j].valid = false;
        }

        douglasPeucker(
            points,
            0,
            points_len - 1,
            5
        );

        int valid_points = 0;
        for(size_t j = 0; j < points_len; j++) {
            DPPoint p = points[j];
            if(!p.valid) continue;
            valid_points++;

            fprintf(
                obj_file,
                "v %f %f %f\n",
                points[j].x,
                points[j].y,
                points[j].z
            );
        }

        if(valid_points == 0) continue;

        fprintf(obj_file, "l ");
        int first = vertexes;

        for (int j = 0; j < valid_points; j++) {
            fprintf(obj_file, "%d ", vertexes++);
        }

        fprintf(obj_file, "%d\n", first);
    }
    fclose(obj_file);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            // write neighbour file
            Pixel pixel = neighbour_pixels[index];
            fputc(pixel.rgb.r, neighbour_file);
            fputc(pixel.rgb.g, neighbour_file);
            fputc(pixel.rgb.b, neighbour_file);
        }
    }

    for(size_t i = 0; i < boundaries.len; i++) {
        Boundary boundary = boundaries.items[i];
        free(boundary.points);
    }
    free(boundaries.items);

    free(neighbour_pixels);
    free(outline_pixels);
    free(gaussian_pixels);

    fclose(neighbour_file);
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Path required.\n");
        return 1;
    }

    char *path = argv[1];
    size_t size = getFileSize(path);

    size_t buffer_size = size + BIT_LENGTH;
    char *buffer = malloc(buffer_size * sizeof(char));

    size_t hex_size = size * 2 + BIT_LENGTH;
    char *hex = malloc(hex_size * sizeof(char));

    size_t hex_len = getHexDump(
        buffer,
        buffer_size,

        hex,
        hex_size,
        path
    );

    int ancillaries_amount = countAncillaries(hex, hex_len);
    ANCI *ancillaries = malloc(ancillaries_amount * sizeof(ANCI));
    if(ancillaries == NULL) {
        printf("Allocation failed (ancillaries).\n");
        exit(0);
    }


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
        .ancillaries = ancillaries,
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

    handleAncillaries(png, ancillaries_amount);

    size_t idats_size = 0;
    for(int i = 0; i < idats_amount; i++) {
        size_t idat_data_size = png.idats[i].size;
        idats_size += idat_data_size;
    }

    // - 4 * 4 (because of the first IDAT)
    size_t binary_size = idats_size * 8 - (4 * 4);
    size_t binary_len = 0;
    char *binary = malloc(binary_size * sizeof(char));
    if(binary == NULL) {
        printf("Allocation failed (binary).\n");
        exit(0);
    }

    for(int i = 0; i < idats_amount; i++) {
        size_t idat_data_size = png.idats[i].size * 2;
        if(i == 0) idat_data_size -= 4;

        size_t binary_idat_data_size = idat_data_size * 4;
        char *binary_idat_data = malloc(binary_idat_data_size * sizeof(char));
        if(binary_idat_data == NULL) {
            printf("Allocation failed (idat).\n");
            exit(0);
        }

        hexToBinary(
            png.idats[i].data,
            idat_data_size,

            binary_idat_data,
            binary_idat_data_size,
            true
        );
        
        writeBufferOffset(
            binary_idat_data,
            binary,
            
            0,
            binary_idat_data_size,

            binary_len // offset
        );
        binary_len += binary_idat_data_size;
        
        free(binary_idat_data);
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

    size_t uncompressed_size = png.ihdr.height * (1 + png.ihdr.width * size_multiplicator);
    int *uncompressed = malloc(uncompressed_size * sizeof(int));
    if(uncompressed == NULL) {
        printf("Allocation failed (uncompressed).\n");
        exit(0);
    }

    size_t uncompressed_position = 0;
    getHuffman(
        binary,
        binary_len,

        uncompressed,
        uncompressed_size,
        &uncompressed_position,
        
        &huffman
    );

    size_t pixels_size = png.ihdr.height * (1 + png.ihdr.width * size_multiplicator);
    Pixel *pixels = malloc(pixels_size * sizeof(Pixel));
    if(pixels == NULL) {
        printf("Allocation failed (pixels).\n");
        exit(0);
    }

    size_t pixels_position = 0;
    getFilter(
        pixels,
        &pixels_position,

        uncompressed, 
        uncompressed_position,
        &png
    );

    writePPM(pixels, pixels_position, png);

    free(buffer);
    free(hex);
    free(binary);
    free(uncompressed);
    free(pixels);
    for(int i = 0; i < idats_amount; i++) {
        free(png.idats[i].data);
    }
    free(idats);
    free(rgbs);
    for(int i = 0; i < ancillaries_amount; i++) {
        free(png.ancillaries[i].data);
    }
    free(ancillaries);

    return 0;
}
