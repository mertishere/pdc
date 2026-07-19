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

bool isInArray(
    int arr[],
    size_t arr_size,
    int x
) {
    for(size_t i = 0; i < arr_size; i++) {
        if(arr[i] == x) return true;
    }

    return false;
}

int _neigbourCheck(
    Pixel *pixels,
    size_t pixel_size,

    int boundarys[],
    size_t boundary_size,
    size_t boundarys_len,

    int index,
    PNG png
) {
    int len = boundarys_len;
    int width = png.ihdr.width;

    int positions[8];

    positions[0] = index + 1;
    positions[1] = index + width + 1;
    positions[2] = index + width;
    positions[3] = index + width - 1;
    positions[4] = index - 1;
    positions[5] = index - width - 1;
    positions[6] = index - width;
    positions[7] = index - width + 1;

    for(int i = 0; i < 8; i++) {
        int pos = positions[i];
        if(pos < 0 || pos >= pixel_size) continue;

        Pixel pixel = pixels[pos];
        if(pixel.rgb.r == RGB_WHITE) continue;

        if(isInArray(boundarys, boundarys_len, pos)) continue;
        if(len >= boundary_size) break;
        boundarys[len] = pos;
        len++;

        len = _neigbourCheck(
            pixels,
            pixel_size,

            boundarys,
            boundary_size,
            len,

            pos,
            png
        );        
    }

    return len;
}

// returns boundaries of neighbours
void neighbourChecks(
    Pixel *pixels,
    size_t pixels_size,

    Boundaries *boundaries,
    PNG png
) {
    int width = png.ihdr.width;
    int height = png.ihdr.height;

    bool *visited = calloc(pixels_size, sizeof(bool));

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int index = y * width + x;
            if(index >= pixels_size) {
                free(visited);
                return;
            }

            if(visited[index]) continue;
            if(pixels[index].rgb.r == RGB_WHITE) continue;

            if (boundaries->len == boundaries->capacity) {
                boundaries->capacity *= 2;
                boundaries->items = realloc(
                    boundaries->items,
                    boundaries->capacity * sizeof(Boundary)
                );
            }

            size_t boundarys_size = 512;
            size_t boundarys_len = 0;
            int boundarys[boundarys_size]; // max

            int p = index;            
            boundarys[boundarys_len] = p;
            boundarys_len++;

            boundarys_len = _neigbourCheck(
                pixels,
                pixels_size,
                boundarys,
                boundarys_size,
                boundarys_len,
                p,
                png
            );
            
            Boundary *b = &boundaries->items[boundaries->len++];
            b->len = boundarys_len;
            b->capacity = boundarys_len;
            b->points = malloc(boundarys_len * sizeof(int));
            memcpy(b->points, boundarys, boundarys_len * sizeof(int));

            for(int i = 0; i < boundarys_len; i++) {
                int pos = boundarys[i];
                if(visited[pos]) continue;
                visited[pos] = true;
            }
        }
    }

    free(visited);
    return;
}
