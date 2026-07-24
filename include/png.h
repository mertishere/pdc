#ifndef PNG_H
#define PNG_H

#include "stddef.h"
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
    char signature[9]; // 8 bytes and 1 NULL byte
    size_t size;
    char *data; // the data for each ancillary chunk needs to be handled independently
} ANCI;

typedef struct {
    char *data;
    size_t size;
    size_t cursor;
} Data;

typedef struct {
    int *data;
    size_t size;
    size_t cursor;
} DataInt;

typedef struct {
    RGB rgb;
    int row;
    int col;
} Pixel;

typedef struct {
    Pixel *data;
    size_t size;
    size_t cursor;
} DataPixel;

typedef struct {
    char *path; // path of the PNG
    size_t size; // size of PNG file
    Data buffer; // the buffer to handle the PNG
    Data hex; // the hex buffer to handle the PNG
    Data binary; // the binary version of the buffer to handle the PNG
    DataInt raw; // the raw version of the buffer to handle the PNG
    DataPixel pixels; // the pixels of the PNG

    int idats_amount;
    int rgbs_amount;
    int ancillaries_amount;

    IHDR ihdr;
    IEND iend;
    PLTE plte;
    IDAT *idats;
    ANCI *ancillaries;
} PNG;

extern PNG parsePng(char *path);
extern void freePng(PNG *png);
#endif