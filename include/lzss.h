#ifndef LZSS_H
#define LZSS_H

#include "huffman.h"

typedef struct {
    int symbol;
    int base;
    int extra;
} Symbol;

extern void handleLzssStatic(
    int symbol,
    char binary[],
    size_t *cbp,
    int uncompressed[],
    size_t *ul
);

extern void handleLzssDynamic(
    int symbol,

    char binary[],
    size_t binary_len,
    
    size_t *cbp,
    
    int uncompressed[],
    size_t *ul,
    
    CodeLengthSymbols hdist_huffman_codes[],
    size_t hdist_huffman_codes_size
);

#endif