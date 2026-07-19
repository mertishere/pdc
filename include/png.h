#ifndef PNG_H
#define PNG_H

#include "stddef.h"

typedef struct {
    int r;
    int g;
    int b;
    int a;
} RGB;

typedef struct {
    size_t size;
    RGB *rgbs;
} PLTE;

typedef struct {
    size_t size;
    size_t width;
    size_t height;
    int bits_per_pixel;
    int color_type;
    int compression_method;
    int filter_method;
    int interlaced;
} IHDR;

typedef struct {
    size_t size;
    int compression_method;
    int compression_info;
    int zlib_fcheck_value;
    int adler_zlib_checksum;
    char *data;
} IDAT;

typedef struct {
    size_t size;
} IEND;

typedef struct {
    IHDR ihdr;
    IEND iend;
    PLTE plte;
    IDAT *idats;
} PNG;

typedef struct {
    RGB rgb;
    int row;
    int col;
} Pixel;

typedef struct {
    PNG png;
    Pixel *pixels;
} Decompressed;

extern int countIdats(char hex[], size_t hex_size);
extern int countRgbs(char hex[], size_t hex_size);

extern void getPng(PNG *png, char hex[], size_t hex_size);

#endif