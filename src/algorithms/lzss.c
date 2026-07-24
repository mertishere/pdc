#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include "constants.h"
#include "algorithms.h"

void setDist(int symbol, Symbol *dist) {
    if (symbol < 0 || symbol >= 30) return;

    dist->symbol = symbol;
    dist->base = BASE_DISTANCES[symbol];
    dist->extra = EXTRA_BITS[symbol];
    return;
}

void setBlock(int symbol, Symbol *block) {
    if(symbol == 256) {
        block->symbol = symbol;
        block->base = 0;
        block->extra = 0;
    } else if(symbol >= 257 && symbol <= 264) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * 0) + (symbol - 257) * 1;
        block->extra = 0;
    } else if(symbol >= 265 && symbol <= 268) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * pow(BINARY_BASE, 0)) + (symbol - 265) * 2;
        block->extra = 1;
    } else if(symbol >= 269 && symbol <= 272) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * pow(BINARY_BASE, 1)) + (symbol - 269) * 4;
        block->extra = 2;
    } else if(symbol >= 273 && symbol <= 276) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * pow(BINARY_BASE, 2)) + (symbol - 273) * 8;
        block->extra = 3;
    } else if(symbol >= 277 && symbol <= 280) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * pow(BINARY_BASE, 3)) + (symbol - 277) * 16;
        block->extra = 4;
    } else if(symbol >= 281 && symbol <= 284) {
        block->symbol = symbol;
        block->base = (DEF_BASE_LENGTH + BYTE_LENGTH * pow(BINARY_BASE, 4)) + (symbol - 281) * 32;
        block->extra = 5;
    } else if(symbol == 285) {
        block->symbol = symbol;
        block->base = 258;
        block->extra = 0;
    } else {
        printf("Invalid symbol.");
        exit(EXIT_FAILURE);
        return;
    }
    return;
}

void handleLzssStatic(
    int symbol,
    PNG *png
) {
    Symbol block_symbol;
    setBlock(symbol, &block_symbol);

    int block_extra_bits = 0;
    if(block_symbol.extra > 0) {
        if (png->binary.cursor + block_symbol.extra > png->binary.size) { 
            png->binary.cursor = png->binary.size; 
            png->raw.cursor = png->raw.size; 
            return; 
        }

        char block_extra_bits_slice[block_symbol.extra];
        writeBuffer(
            png->binary.data,
            block_extra_bits_slice,
            png->binary.cursor,
            png->binary.cursor + block_symbol.extra
        );
        block_extra_bits = binaryToInt(block_extra_bits_slice, block_symbol.extra, true);
        png->binary.cursor += block_symbol.extra;
    }


    int total_copies = block_symbol.base + block_extra_bits;
    if (png->binary.cursor + 5 > png->binary.size) { 
        png->binary.cursor = png->binary.size;
        png->raw.cursor = png->raw.size;
        return; 
    }

    char distance_symbol_slice[5];
    writeBuffer(png->binary.data, distance_symbol_slice, png->binary.cursor, png->binary.cursor + 5);
    png->binary.cursor += 5;
    int distance_symbol = binaryToInt(distance_symbol_slice, 5, false);
    
    Symbol dist_symbol = {};
    setDist(distance_symbol, &dist_symbol);

    int distance_extra_bits = 0;
    if(dist_symbol.extra > 0) {
        if (png->binary.cursor + dist_symbol.extra > png->binary.size) { 
            png->binary.cursor = png->binary.size;
            png->raw.cursor = png->raw.size;
            return; 
        }
        char distance_symbol_extra_slice[dist_symbol.extra];
        writeBuffer(
            png->binary.data,
            distance_symbol_extra_slice,
            png->binary.cursor,
            png->binary.cursor + dist_symbol.extra
        );
        png->binary.cursor += dist_symbol.extra;
        distance_extra_bits = binaryToInt(distance_symbol_extra_slice, dist_symbol.extra, true);
    }

    size_t total_distance = dist_symbol.base + distance_extra_bits;
    if (png->raw.cursor < total_distance) {
        printf(
            "1. WARNING TOTAL DISTANCE FAR EXCEEDS CURRENT LENGTH! CL: %ld | TD: %ld\n",
            png->raw.cursor,
            total_distance
        );
        exit(EXIT_FAILURE);
        return;
    }

    for (
        size_t i = png->raw.cursor - total_distance;
        i < png->raw.cursor - total_distance + total_copies;
        i++
    ) {
        png->raw.data[png->raw.cursor] = png->raw.data[i];
        png->raw.cursor++;
        png->raw.data[png->raw.cursor] = '\0';
    }
}

void handleLzssDynamic(
    int symbol,
    CodeLengthSymbols hdist_huffman_codes[],
    size_t hdist_huffman_codes_size,
    PNG *png
) {
    Symbol block_symbol;
    setBlock(symbol, &block_symbol);

    int block_extra_bits = 0;
    if(block_symbol.extra > 0) {
        char block_extra_bits_slice[block_symbol.extra];
        if(png->binary.cursor + block_symbol.extra > png->binary.size) {
            png->binary.cursor = png->binary.size;
            png->raw.cursor = png->raw.size;
            return;
        }

        writeBuffer(png->binary.data, block_extra_bits_slice, png->binary.cursor, png->binary.cursor + block_symbol.extra);
        block_extra_bits = binaryToInt(block_extra_bits_slice, block_symbol.extra, true);
        png->binary.cursor += block_symbol.extra;
    }
    int total_copies = block_symbol.base + block_extra_bits;

    int distance_symbol_storer[16];
    int distance_symbol_storer_size = 0;
    int distance_symbol = -1;

    while (png->binary.cursor < png->binary.size) {
        distance_symbol_storer[distance_symbol_storer_size] = png->binary.data[png->binary.cursor] - INT_TO_ASCII_OFFSET;
        distance_symbol_storer_size++;
        if(distance_symbol_storer_size < 4) {
            distance_symbol_storer[distance_symbol_storer_size] = '\0';
        }

        png->binary.cursor += 1;
        for (size_t i = 0; i < hdist_huffman_codes_size; i++) {
            CodeLengthSymbols hdist_huffman_code = hdist_huffman_codes[i];
            if (cmpInts(
                hdist_huffman_code.code,
                hdist_huffman_code.len,
                distance_symbol_storer,
                distance_symbol_storer_size
            )) {
                distance_symbol = hdist_huffman_code.symbol;
                break;
            }
            continue;
        }

        if (distance_symbol < 0) continue;
        break;
    }

    Symbol dist_symbol = {};

    setDist(distance_symbol, &dist_symbol);

    if (png->binary.cursor + dist_symbol.extra > png->binary.size) { 
        png->binary.cursor = png->binary.size;
        png->raw.cursor = png->raw.size;
        return; 
    }

    char distance_extra_bits_slice[dist_symbol.extra == 0 ? 1 : dist_symbol.extra];
    writeBuffer(
        png->binary.data,
        distance_extra_bits_slice,
        png->binary.cursor,
        png->binary.cursor + dist_symbol.extra
    );

    int distance_extra_bits = binaryToInt(distance_extra_bits_slice, dist_symbol.extra, true);
    png->binary.cursor += dist_symbol.extra;
    
    size_t total_distance = dist_symbol.base + distance_extra_bits;
    if (png->raw.cursor < total_distance) {
        printf(
            "2. WARNING TOTAL DISTANCE FAR EXCEEDS CURRENT LENGTH! CL: %ld | TD: %ld\n",
            png->raw.cursor,
            total_distance
        );
        exit(EXIT_FAILURE);
        return;
    }

    size_t start = png->raw.cursor - total_distance;
    size_t end = start + total_copies;

    for (size_t i = start; i < end; i++) {
        if(png->raw.cursor >= png->raw.size) {
            printf("Too much.\n");
            break;
        }

        int copy = png->raw.data[i];
        png->raw.data[png->raw.cursor] = copy;
        png->raw.cursor++;
    }
}
