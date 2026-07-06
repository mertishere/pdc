#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include "constants.h"
#include "lzss.h"
#include "huffman.h"

void setDist(int symbol, Symbol *dist) {
    if (symbol >= 30) return;

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
    char binary[],
    size_t *cbp,
    
    int uncompressed[],
    size_t *ul
) {
    size_t current_bit_position = *cbp;
    size_t current_complete_blocks_position = *ul;

    Symbol block_symbol;
    setBlock(symbol, &block_symbol);

    char block_extra_bits_slice[block_symbol.extra];
    writeBuffer(binary, block_extra_bits_slice, current_bit_position, current_bit_position + block_symbol.extra);

    int block_extra_bits = binaryToInt(block_extra_bits_slice, block_symbol.extra, true);
    current_bit_position += block_symbol.extra;

    int total_copies = block_symbol.base + block_extra_bits;

    char distance_symbol_slice[5];
    writeBuffer(binary, distance_symbol_slice, current_bit_position, current_bit_position + 5);
    current_bit_position += 5;
    int distance_symbol = binaryToInt(distance_symbol_slice, 5, false);
    
    Symbol dist_symbol;
    dist_symbol.base = 0;
    dist_symbol.extra = 0;
    dist_symbol.symbol = 0;

    setDist(distance_symbol, &dist_symbol);

    char distance_symbol_extra_slice[dist_symbol.extra];
    writeBuffer(binary, distance_symbol_extra_slice, current_bit_position, current_bit_position + dist_symbol.extra);
    current_bit_position += dist_symbol.extra;
    int distance_extra_bits = binaryToInt(distance_symbol_extra_slice, dist_symbol.extra, true);

    int total_distance = dist_symbol.base + dist_symbol.extra;

    if (current_complete_blocks_position < total_distance) {
        printf("1. WARNING TOTAL DISTANCE FAR EXCEEDS CURRENT LENGTH! CL: %d | TD: %d\n", current_complete_blocks_position, total_distance);
        exit(EXIT_FAILURE);
        return;
    }

    for (int i = current_complete_blocks_position - total_distance; i < current_complete_blocks_position - total_distance + total_copies; i++) {
        uncompressed[current_complete_blocks_position] = uncompressed[i];
        current_complete_blocks_position++;
        uncompressed[current_complete_blocks_position] = '\0';
    }

    *cbp = current_bit_position;
    *ul = current_complete_blocks_position;
}

void handleLzssDynamic(
    int symbol,

    char binary[],
    size_t binary_len,

    size_t *cbp,
    
    int uncompressed[],
    size_t *ul,
    
    CodeLengthSymbols hdist_huffman_codes[],
    size_t hdist_huffman_codes_size
) {
    size_t current_bit_position = *cbp;
    size_t uncompressed_len = *ul;
    
    Symbol block_symbol;
    setBlock(symbol, &block_symbol);

    char block_extra_bits_slice[block_symbol.extra];
    writeBuffer(binary, block_extra_bits_slice, current_bit_position, current_bit_position + block_symbol.extra);
    int block_extra_bits = binaryToInt(block_extra_bits_slice, block_symbol.extra, true);
    current_bit_position += block_symbol.extra;
    int total_copies = block_symbol.base + block_extra_bits;

    int distance_symbol_storer[16];
    int distance_symbol_storer_size = 0;
    int distance_symbol = -1;

    while (current_bit_position < binary_len) {
        distance_symbol_storer[distance_symbol_storer_size] = binary[current_bit_position] - INT_TO_ASCII_OFFSET;
        distance_symbol_storer_size++;
        if(distance_symbol_storer_size < 4) {
            distance_symbol_storer[distance_symbol_storer_size] = '\0';
        }

        current_bit_position += 1;
        for (int i = 0; i < hdist_huffman_codes_size; i++) {
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

    Symbol dist_symbol;
    dist_symbol.base = 0;
    dist_symbol.extra = 0;
    dist_symbol.symbol = 0;

    setDist(distance_symbol, &dist_symbol);

    char distance_extra_bits_slice[dist_symbol.extra];
    writeBuffer(
        binary,
        distance_extra_bits_slice,
        current_bit_position,
        current_bit_position + dist_symbol.extra
    );

    int distance_extra_bits = binaryToInt(distance_extra_bits_slice, dist_symbol.extra, true);
    current_bit_position += dist_symbol.extra;
    
    int total_distance = dist_symbol.base + distance_extra_bits;
    if (uncompressed_len < total_distance) {
        printf(
            "2. WARNING TOTAL DISTANCE FAR EXCEEDS CURRENT LENGTH! CL: %d | TD: %d\n",
            uncompressed_len,
            total_distance
        );
        exit(EXIT_FAILURE);
        return;
    }

    size_t start = uncompressed_len - total_distance;
    size_t end = start + total_copies;

    for (int i = start; i < end; i++) {
        int copy = uncompressed[i];
        uncompressed[uncompressed_len] = copy;
        uncompressed_len++;
    }

    // uncompressed[uncompressed_len] = '\0';

    *ul = uncompressed_len;
    *cbp = current_bit_position;
}
