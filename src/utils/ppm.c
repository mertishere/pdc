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

void writePPM(PNG png) {
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

    size_t width = png.ihdr.width;
    size_t height = png.ihdr.height;

    size_t pixel_size = png.pixels.size;
    Pixel *pixels = png.pixels.data;


    // P6 header
    fprintf(normal_file, "P6\n%ld %ld\n255\n", width, height);
    fprintf(neighbour_file, "P6\n%ld %ld\n255\n", width, height);
    fprintf(gaussian_file, "P6\n%ld %ld\n255\n", width, height);

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

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            size_t index = y * width + x;

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
