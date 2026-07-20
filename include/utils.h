#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "png.h"

extern void intToBinary(int byte, char *buffer, size_t size);
extern void hexToBinary(
    char *hex,
    size_t hex_size,
    char *buffer,
    size_t buffer_size,
    bool lsb
);

extern void intToHex(int byte, char *buffer, size_t size);
extern void binaryToHex(char *binary, char *buffer, size_t size);

extern int hexToInt(char *hex, size_t size);
extern int binaryToInt(char *binary, size_t size, bool lsb);

extern int powOfInt(int byte);

extern void removeWhitespace(char buffer[], size_t size);
extern void reverseString(char buffer[], size_t size);

extern size_t getHexDump(char buffer[], size_t buffer_size, char hex[], size_t hex_size, char *file_path);
extern void hexMultiple(char buffer[], size_t buffer_size, char hex[], size_t hex_size);
extern void hexDump(char buffer[], size_t buffer_size, char hex[], size_t hex_size);

extern void writeBuffer(char src[], char out[], size_t start, size_t end);
extern void writeBufferOffset(char src[], char out[], size_t start, size_t end, size_t offset);

extern bool cmpInts(int c1[], int len1, int c2[], int len2);
extern size_t getFileSize(char *path);

typedef struct {
    float x;
    float y;
    float z;
    bool valid;
} DPPoint;

extern void douglasPeucker(
    DPPoint *points,
    int start,
    int end,
    float epsilon
);

#endif