#ifndef HUFFMAN_H
#define HUFFMAN_H

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
    size_t *ul,
    Huffman *huffman
);

#endif