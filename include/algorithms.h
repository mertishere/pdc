#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "png.h"

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

typedef struct {
    int symbol;
    int base;
    int extra;
} Symbol;

extern void getHuffman(PNG *png);
extern void getFilter(PNG *png);

extern void handleLzssStatic(
    int symbol,
    PNG *png
);

extern void handleLzssDynamic(
    int symbol,
    CodeLengthSymbols hdist_huffman_codes[],
    size_t hdist_huffman_codes_size,
    PNG *png
);

extern void handleAncillaries(PNG png);

#endif