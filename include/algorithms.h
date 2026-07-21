#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "png.h"

extern void getFilter(
    Pixel pixels[],
    size_t *pixels_len, 
    
    int uncompressed_data[], 
    size_t uncompressed_data_len, 
    PNG *png
);

typedef struct {
    int btype;
    int bfinal;
    int hclen;
    int hdist;
    int hlit;
} Huffman;

typedef struct {
    int bits[16];
    int length;
} HuffmanCode;

typedef struct {
    int symbol;
    int len;
    int code[16];
} CodeLengthSymbols;

extern void getHuffman(
    char binary[],
    size_t binary_len,
    int uncompressed[],
    size_t uncompressed_len,
    size_t *ul,
    Huffman *huffman
);

typedef struct {
    int symbol;
    int base;
    int extra;
} Symbol;

extern void handleLzssStatic(
    int symbol,
    char binary[],
    size_t binary_len,
    size_t *bp,

    int uncompressed[],
    size_t *up
);

extern void handleLzssDynamic(
    int symbol,

    char binary[],
    size_t binary_len,
    size_t *bp,
    
    int uncompressed[],
    size_t uncompressed_len,
    size_t *up,
    
    CodeLengthSymbols hdist_huffman_codes[],
    size_t hdist_huffman_codes_size
);

extern void handleAncillaries(PNG png, size_t ancillaries_amount);

#endif