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
    PNG *png,
    Pixel *outline_pixels,

    int boundarys[],
    size_t boundary_size,
    size_t boundarys_len,
    int index
) {
    // if(isInArray(boundarys, boundarys_len, index)) return boundarys_len;

    size_t len = boundarys_len;
    int positions[8];

    positions[0] = index + 1;
    positions[1] = index + png->ihdr.width + 1;
    positions[2] = index + png->ihdr.width;
    positions[3] = index + png->ihdr.width - 1;
    positions[4] = index - 1;
    positions[5] = index - png->ihdr.width - 1;
    positions[6] = index - png->ihdr.width;
    positions[7] = index - png->ihdr.width + 1;

    int similarities = 0;
    for(int i = 0; i < 8; i++) {
        int pos = positions[i];
        if(pos < 0 || (size_t)pos >= png->pixels.size) continue;

        Pixel pixel = outline_pixels[pos];
        if(pixel.rgb.r == RGB_WHITE) continue;

        if(isInArray(boundarys, boundarys_len, pos)) {
            similarities++;
            continue;
        }
        if(similarities > 4) break;
        if(len >= boundary_size) break;
        boundarys[len] = pos;
        len++;

        len = _neigbourCheck(
            png,
            outline_pixels,

            boundarys,
            boundary_size,
            len,

            pos
        );
    }

    return len;
}

// returns boundaries of neighbours
void neighbourChecks(
    PNG *png,
    Pixel *outline_pixels,
    Boundaries *boundaries
) {
    bool *visited = calloc(png->pixels.size, sizeof(bool));

    for (size_t y = 1; y < png->ihdr.height - 1; y++) {
        for (size_t x = 1; x < png->ihdr.width - 1; x++) {
            size_t index = y * png->ihdr.width + x;
            if(index >= png->pixels.size) {
                free(visited);
                return;
            }

            if(visited[index]) continue;
            if(outline_pixels[index].rgb.r == RGB_WHITE) continue;

            if (boundaries->len == boundaries->capacity) {
                boundaries->capacity *= 2;
                boundaries->items = realloc(
                    boundaries->items,
                    boundaries->capacity * sizeof(Boundary)
                );
            }

            size_t boundarys_size = 512 * 8;
            size_t boundarys_len = 0;
            int boundarys[boundarys_size]; // max

            int p = index;            
            boundarys[boundarys_len] = p;
            boundarys_len++;

            boundarys_len = _neigbourCheck(
                png,
                outline_pixels,

                boundarys,
                boundarys_size,
                boundarys_len,
                p
            );
            
            Boundary *b = &boundaries->items[boundaries->len++];
            b->len = boundarys_len;
            b->capacity = boundarys_len;
            b->points = malloc(boundarys_len * sizeof(int));
            memcpy(b->points, boundarys, boundarys_len * sizeof(int));

            for(size_t i = 0; i < boundarys_len; i++) {
                int pos = boundarys[i];
                if(visited[pos]) continue;
                visited[pos] = true;
            }
        }
    }

    free(visited);
    return;
}
