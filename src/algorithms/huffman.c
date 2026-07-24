
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "algorithms.h"
#include "constants.h"
#include "utils.h"

#define HCLEN_SIZE 19
// #define BIT_LENGTH 1
#define BYTE_LENGTH 8

void sort(
    int src[],
    int out[],
    int len
) {
    if(len == 0) return;

    int indexes[len];
    for(int i = 0; i < len; i++) {
        indexes[i] = -1;
    }

    int indexes_len = 0;

    for(int i = 0; i < len; i++) {
        int index = -1;

        for (int j = 0; j < len; j++) {
            bool used = false;

            for (int k = 0; k < indexes_len; k++) {
                if (indexes[k] == j) {
                    used = true;
                    break;
                }
            }

            if (!used) {
                index = j;
                break;
            }
        }

        int max = src[index];

        for(int j = 0; j < len; j++) {
            if(max > src[j]) {
                bool used = false;
                for(int k = 0; k < indexes_len; k++) {
                    if(indexes[k] == j) {
                        used = true;
                        break;
                    }
                }
                if(used) continue;

                max = src[j];
                index = j;
            }
        }

        out[indexes_len] = max;
        indexes[indexes_len] = index;
        indexes_len++;
    }
}

void handleHuffmanCodeCreation(
    HuffmanCode out_code[],

    int code_lengths[],
    int code_lengths_len
) {
    int out_code_len = 0;

    int code_lengths_sorted[code_lengths_len];
    sort(code_lengths, code_lengths_sorted, code_lengths_len);

    for (int i = 0; i < code_lengths_len; i++) {
        int code_length = code_lengths_sorted[i];
        if (code_length == 0) continue;

        int create_code_len = 0;
        if (out_code_len > 0) {
            HuffmanCode last_code = out_code[out_code_len - 1];
            for (int j = 0; j < last_code.length; j++) {
                out_code[out_code_len].bits[create_code_len] = last_code.bits[j];
                create_code_len++;
            }
        } else {
            for (int j = 0; j < code_length; j++) {
                out_code[out_code_len].bits[create_code_len] = 0;
                create_code_len++;
            }
            out_code[out_code_len].length = create_code_len;
            out_code_len++;
            continue;
        }

        for (int j = 0; j < create_code_len; j++) {
            int index = create_code_len - j - 1;
            int bit = out_code[out_code_len].bits[index];
            int replace_bit = bit == 0 ? 1 : 0;

            out_code[out_code_len].bits[index] = replace_bit;
            if (replace_bit == 1) break;
        }

        if (out_code_len > 0) {
            HuffmanCode last_code = out_code[out_code_len - 1];

            for (int j = 0; j < code_length - last_code.length; j++) {
                out_code[out_code_len].bits[create_code_len] = 0;
                create_code_len++;
            }
        }

        out_code[out_code_len].length = create_code_len;
        
        out_code_len++;
    }
}

void buildHuffman(
    int builder[],
    int builder_len,
    
    CodeLengthSymbols cls[],
    int cls_len
) {
    int code_lengths_huffman_len = builder_len;
    int code_lengths_huffman[code_lengths_huffman_len];

    for (int i = 0; i < code_lengths_huffman_len; i++) { // the builder contains the lengths of the bits for each code length
        int bit_length = builder[i];
        code_lengths_huffman[i] = bit_length;
    }

    int used_huffman_codes[builder_len];
    int used_huffman_codes_len = 0;

    HuffmanCode huffman_codes[builder_len]; 
    handleHuffmanCodeCreation(huffman_codes, code_lengths_huffman, code_lengths_huffman_len);

    int current_cls_len = 0;
    for (int i = 0; i < code_lengths_huffman_len; i++) {
        int code_length = code_lengths_huffman[i];
        if (code_length == 0) continue;

        for (int j = 0; j < builder_len; j++) {
            HuffmanCode huffman_code = huffman_codes[j];
            if (huffman_code.length == 0) continue;
            if (huffman_code.length != code_length) continue;

            bool used = false;
            for(int k = 0; k < used_huffman_codes_len; k++) {
                if(used_huffman_codes[k] == j) {
                    used = true;
                    break;
                }
            }
            if (used) continue;
            used_huffman_codes[used_huffman_codes_len] = j;
            used_huffman_codes_len++;


            cls[current_cls_len].symbol = i;
            cls[current_cls_len].len = code_length;
            for(int k = 0; k < code_length; k++) {
                cls[current_cls_len].code[k] = huffman_code.bits[k];
            }
            
            current_cls_len++;
            if(current_cls_len == cls_len) return;
            break;
        }
    }
}

void noHuffman(PNG *png) {
    // skip the 5 bits to make a full byte out of the header |BFINAL (1b)|BTYPE(2b)|...(5b)|LEN|NLEN|...
    png->binary.cursor += 5;

    if (png->binary.cursor + BYTE_LENGTH * 2 > png->binary.size) { 
        png->binary.cursor = png->binary.size;
        png->raw.cursor = png->raw.cursor; 
        return;
    }

    char len_slice[BYTE_LENGTH * 2 + 1];
    writeBuffer(png->binary.data, len_slice, png->binary.cursor, png->binary.cursor + BYTE_LENGTH * 2);
    len_slice[BYTE_LENGTH * 2] = '\0';

    int len = binaryToInt(len_slice, BYTE_LENGTH * 2, true);
    png->binary.cursor += BYTE_LENGTH * 2;

    if (png->binary.cursor + BYTE_LENGTH * 2 > png->binary.size) { 
        png->binary.cursor = png->binary.size;
        png->raw.cursor = png->raw.cursor; 
        return;
    }

    char nlen_slice[BYTE_LENGTH * 2 + 1];
    writeBuffer(png->binary.data, nlen_slice, png->binary.cursor, png->binary.cursor + BYTE_LENGTH * 2);
    nlen_slice[BYTE_LENGTH * 2] = '\0';

    int nlen = binaryToInt(nlen_slice, BYTE_LENGTH * 2, true);
    png->binary.cursor += BYTE_LENGTH * 2;

    if (len + nlen != 65535) {
        printf("Invalid LEN and NLEN.\n");
        exit(EXIT_FAILURE);
        return;
    }

    int enumerate = 0;
    while (enumerate < len && (png->binary.cursor + BYTE_LENGTH) <= png->binary.size) {
        char data_block_slice[BYTE_LENGTH];
        writeBuffer(png->binary.data, data_block_slice, png->binary.cursor, png->binary.cursor + BYTE_LENGTH);
        png->binary.cursor += BYTE_LENGTH;
        int parsed_data_block = binaryToInt(data_block_slice, BYTE_LENGTH, true);
        png->raw.data[png->raw.cursor] = parsed_data_block;
        png->raw.cursor++;
        png->raw.data[png->raw.cursor] = '\0';
        enumerate += 1;
    }
}

void staticHuffman(PNG *png) {
    // fixed huffman tree
    // 0 - 143 | 8 bits   (00110000 - 10111111) (48 - 191)
    // 144 - 255 | 9 bits (110010000 - 111111111) (400 - 511)
    // 256 - 279 | 7 bits (0000000 - 0010111) (0 - 23)   POS
    // 280 - 287 | 8 bits (11000000 - 11000111) (192 - 199)   POS
    // Note: 286 and 287 have codes assigned but are RESERVED and must not appear in compressed data.
    char first_seven_slice[7];
    char first_eight_slice[8];
    char first_nine_slice[9];

    while (png->binary.cursor < png->binary.size) {
        writeBuffer(png->binary.data, first_seven_slice, png->binary.cursor, png->binary.cursor + 7);
        int first_seven = binaryToInt(first_seven_slice, 7, false);
        if(first_seven >= 0 && first_seven <= 23) {
            png->binary.cursor += 7;

            int symbol = 256 + first_seven;
            if (symbol != 256) {
                handleLzssStatic(
                    symbol,
                    png
                );
                continue;
            } else break;
        }

        writeBuffer(png->binary.data, first_eight_slice, png->binary.cursor, png->binary.cursor + 8);
        int first_eight = binaryToInt(first_eight_slice, 8, false);

        if (first_eight >= 48 && first_eight <= 191) {
            png->binary.cursor += 8;

            png->raw.data[png->raw.cursor] = 0 + first_eight - 48;
            png->raw.cursor++;
            png->raw.data[png->raw.cursor] = '\0';
            continue;
        }

        // 280 - 287 | 8 bits (11000000 - 11000111) (192 - 199)
        if (first_eight >= 192 && first_eight <= 199) {
            png->binary.cursor += 8;

            int symbol = 280 + (first_eight - 192);
            if (symbol > 285) {
                printf("Invalid symbol.");
                exit(EXIT_FAILURE);
            }

            handleLzssStatic(
                symbol,
                png
            );
            continue;
        }

        writeBuffer(png->binary.data, first_nine_slice, png->binary.cursor, png->binary.cursor + 9);
        int first_nine = binaryToInt(first_nine_slice, 9, false);
        if (first_nine >= 400 && first_nine <= 511) {
            png->binary.cursor += 9;

            png->raw.data[png->raw.cursor] = 144 + first_nine - 400;
            png->raw.cursor++;
            png->raw.data[png->raw.cursor] = '\0';
            continue;
        }
    }
}

void symbolsBuilder(
    PNG *png,

    CodeLengthSymbols cls[],
    int cls_length,
    
    int limit,
    int builder[]
) {
    //    0 - 15: Represent code lengths of 0 - 15
    //        16: Copy the previous code length 3 - 6 times.
    //            The next 2 bits indicate repeat length
    //                  (0 = 3, ... , 3 = 6)

    //               Example:  Codes 8, 16 (+2 bits 11),
    //                         16 (+2 bits 10) will expand to
    //                         12 code lengths of 8 (1 + 6 + 5)

    //        17: Repeat a code length of 0 for 3 - 10 times.
    //            (3 bits of length)

    //        18: Repeat a code length of 0 for 11 - 138 times
    //            (7 bits of length)

    int builder_len = 0;

    int bit_storer[16];
    int bit_storer_len = 0;

    while (builder_len < limit) {
        char bit = png->binary.data[png->binary.cursor];
        png->binary.cursor += BIT_LENGTH;
        bit_storer[bit_storer_len] = bit - INT_TO_ASCII_OFFSET;
        bit_storer_len++;

        int hclen_symbol = -1;
        for (int i = 0; i < cls_length; i++) {
            CodeLengthSymbols c = cls[i];
            if (cmpInts(c.code, c.len, bit_storer, bit_storer_len)) {
                hclen_symbol = c.symbol;
                break;
            }
            continue;
        }

        if (hclen_symbol != -1) {
            if (hclen_symbol < 16) {
                builder[builder_len] = hclen_symbol;
                builder_len++;
            }
            if (hclen_symbol == 16) {
                int previous_code = builder[builder_len - 1];

                char extra_bits_slice[2];
                writeBuffer(png->binary.data, extra_bits_slice, png->binary.cursor, png->binary.cursor + 2);
                int extra_bits = binaryToInt(extra_bits_slice, 2, true);
                png->binary.cursor += 2;

                int total_repeat = extra_bits + 3;
                for (int i = 0; i < total_repeat; i++) {
                    builder[builder_len] = previous_code;
                    builder_len++;
                }
            }
            if (hclen_symbol == 17) {
                int previous_code = 0;
                
                char extra_bits_slice[3];
                writeBuffer(png->binary.data, extra_bits_slice, png->binary.cursor, png->binary.cursor + 3);
                int extra_bits = binaryToInt(extra_bits_slice, 3, true);
                png->binary.cursor += 3;

                int total_repeat = extra_bits + 3;
                for (int i = 0; i < total_repeat; i++) {
                    builder[builder_len] = previous_code;
                    builder_len++;
                }
            }
            if (hclen_symbol == 18) {
                int previous_code = 0;

                char extra_bits_slice[7];
                writeBuffer(png->binary.data, extra_bits_slice, png->binary.cursor, png->binary.cursor + 7);
                int extra_bits = binaryToInt(extra_bits_slice, 7, true);
                png->binary.cursor += 7;

                int total_repeat = extra_bits + 11;
                for (int i = 0; i < total_repeat; i++) {
                    builder[builder_len] = previous_code;
                    builder_len++;
                }
            }
            bit_storer_len = 0;
            bit_storer[bit_storer_len] = '\0';
        }
    }
}

void huffmanBuilder(
    char binary[],
    size_t binary_len,

    Huffman *huffman,
    size_t *bp
) {
    size_t binary_position = *bp;

    huffman->hlit = 257;
    huffman->hdist = 1;
    huffman->hclen = 4;

    if (binary_position + 5 > binary_len) { *bp = binary_position; return; }

    char hlit[5];
    writeBuffer(binary, hlit, binary_position, binary_position + 5);
    binary_position += 5;
    huffman->hlit = 257 + binaryToInt(hlit, 5, true);

    if (binary_position + 5 > binary_len) { *bp = binary_position; return; }

    char hdist[5];
    writeBuffer(binary, hdist, binary_position, binary_position + 5);
    binary_position += 5;
    huffman->hdist = 1 + binaryToInt(hdist, 5, true);

    if (binary_position + 4 > binary_len) { *bp = binary_position; return; }

    char hclen[4];
    writeBuffer(binary, hclen, binary_position, binary_position + 4);
    binary_position += 4;
    huffman->hclen = 4 + binaryToInt(hclen, 4, true);

    *bp = binary_position;
}

void dynamicHuffman(
    PNG *png,
    Huffman *huffman
) {
    huffmanBuilder(png->binary.data, png->binary.size, huffman, &png->binary.cursor);
    // HCLEN length
    //
    // 19 hclen order length
    int code_length_count = 0;
    int code_lengths[HCLEN_SIZE];
    for(int i = 0; i < HCLEN_SIZE; i++) {
        code_lengths[i] = 0;
    }

    for (int i = 0; i < huffman->hclen; i++) {
        if (png->binary.cursor + 3 > png->binary.size) break;

        char list_code_length[3];
        writeBuffer(
            png->binary.data,
            list_code_length,
            png->binary.cursor,
            png->binary.cursor + 3
        );
        png->binary.cursor += 3;
        int code_length_int = binaryToInt(list_code_length, 3, true);

        if(code_length_int > 0) code_length_count++;

        code_lengths[HCLEN_ORDER[i]] = code_length_int;
    }

    HuffmanCode code_creation[code_length_count > 0 ? code_length_count : 1];
    handleHuffmanCodeCreation(code_creation, code_lengths, HCLEN_SIZE);

    int used_indexes[HCLEN_SIZE];
    for(int i = 0; i < HCLEN_SIZE; i++) {
        used_indexes[i] = -1;
    }
    int used_indexes_len = 0;

    HuffmanCode huffman_codes[HCLEN_SIZE];
    int huffman_codes_len = 0;

    for (int i = 0; i < HCLEN_SIZE; i++) {
        int current_code_length = code_lengths[i];
        if (current_code_length == 0) {
            huffman_codes[huffman_codes_len].length = 0;
            huffman_codes_len++;
            continue;
        }

        for (int j = 0; j < code_length_count; j++) {
            bool index_used = false;
            for(int k = 0; k < code_length_count; k++) {
                if(used_indexes[k] == j) {
                    index_used = true;
                    break;
                }
            }
            if(index_used) continue;

            HuffmanCode huffman_code = code_creation[j];
            if (current_code_length == huffman_code.length) {
                used_indexes[used_indexes_len] = j;
                used_indexes_len++;

                for(int k = 0; k < huffman_code.length; k++) {
                    huffman_codes[huffman_codes_len].bits[k] = huffman_code.bits[k];
                }
                huffman_codes[huffman_codes_len].length = huffman_code.length;
                huffman_codes_len++;
                break;
            }
        }
    }

    
    int cls_size = HCLEN_SIZE;
    CodeLengthSymbols cls[cls_size];
    for(int i = 0; i < cls_size; i++) {
        cls[i].symbol = 0;
        cls[i].len = 0;
    }

    for (int i = 0; i < cls_size; i++) {
        if (code_lengths[i] == 0) continue;
        
        HuffmanCode huffman_code = huffman_codes[i];
        int bits_length = code_lengths[i];
        
        cls[i].symbol = i;
        cls[i].len = bits_length;
        for(int j = 0; j < huffman_code.length; j++) {
            cls[i].code[j] = huffman_code.bits[j];
        }
    }

    int hlit_builder[huffman->hlit];
    for(int i = 0; i < huffman->hlit; i++) {
        hlit_builder[i] = 0;
    }

    symbolsBuilder(
        png,

        cls,
        cls_size,

        huffman->hlit, 
        hlit_builder
    );

    int hlit_huffman_codes_size = 0;
    for(int i = 0; i < huffman->hlit; i++) {
        if(hlit_builder[i] != 0) hlit_huffman_codes_size++;
    }

    CodeLengthSymbols hlit_huffman_codes[hlit_huffman_codes_size > 0 ? hlit_huffman_codes_size : 1];
    for(int i = 0; i < hlit_huffman_codes_size; i++) {
        for(int j = 0; j < 16; j++) {
            hlit_huffman_codes[i].code[j] = 0;
        }

        hlit_huffman_codes[i].symbol = 0;
        hlit_huffman_codes[i].len = 0;
    }

    buildHuffman(
        hlit_builder,
        huffman->hlit,
        
        hlit_huffman_codes,
        hlit_huffman_codes_size
    );

    int hdist_builder[huffman->hdist];
    for(int i = 0; i < huffman->hdist; i++) {
        hdist_builder[i] = 0;
    }

    symbolsBuilder(
        png,

        cls,
        HCLEN_SIZE,
        
        huffman->hdist,
        hdist_builder
    );

    int hdist_huffman_codes_size = 0;
    for(int i = 0; i < huffman->hdist; i++) {
        if(hdist_builder[i] != 0) hdist_huffman_codes_size++;
    }

    CodeLengthSymbols hdist_huffman_codes[hdist_huffman_codes_size > 0 ? hdist_huffman_codes_size : 1];
    for(int i = 0; i < hdist_huffman_codes_size; i++) {
        hdist_huffman_codes[i].symbol = 0;
        hdist_huffman_codes[i].len = 0;
    }

    buildHuffman(
        hdist_builder,
        huffman->hdist,
        
        hdist_huffman_codes,
        hdist_huffman_codes_size
    );


    // ^ everything up here correct
    // ------

    int hlit_bit_storer_size = 16;
    int hlit_bit_storer[hlit_bit_storer_size];
    int hlit_bit_storer_len = 0;

    while (png->binary.cursor < png->binary.size) {
        char bit = png->binary.data[png->binary.cursor];
        png->binary.cursor += BIT_LENGTH;

        if(hlit_bit_storer_len >= hlit_bit_storer_size) {
            printf("Returned bit=%ld\n", png->binary.cursor);
            printf("HLIT bit storer exceeded HCLEN size.\n");
            exit(EXIT_FAILURE);
        }

        hlit_bit_storer[hlit_bit_storer_len] = bit - INT_TO_ASCII_OFFSET;
        hlit_bit_storer_len++;

        int hlit_symbol = -1;

        for (int i = 0; i < hlit_huffman_codes_size; i++) {
            CodeLengthSymbols hlit_huffman_code = hlit_huffman_codes[i];
            if(cmpInts(hlit_huffman_code.code, hlit_huffman_code.len, hlit_bit_storer, hlit_bit_storer_len)) {
                hlit_symbol = hlit_huffman_code.symbol;
                break;
            }
            continue;
        }
        if (hlit_symbol < 0) continue;

        hlit_bit_storer_len = 0;

        if (hlit_symbol == 256) break;
        if (hlit_symbol < 256) {
            png->raw.data[png->raw.cursor] = hlit_symbol;
            png->raw.cursor++;
            continue;
        }

        handleLzssDynamic(
            hlit_symbol,
            hdist_huffman_codes,
            hdist_huffman_codes_size,
            png
        );
    }
}

void handleBtype(
    PNG *png,
    Huffman *huffman
) {
    switch (huffman->btype) {
        case 0:
            noHuffman(png);
            break;
        case 1:
            staticHuffman(png);
            break;
        case 2:
            dynamicHuffman(png, huffman);
            break;
        default:
            printf("Invalid huffman.\n");
            exit(EXIT_FAILURE);
            break;
    }
}

void getHuffman(PNG *png) {
    Huffman huffman = {};

    png->binary.cursor = 0;
    while (true) {
        // Check for the BFINAL and BTYPE
        if (png->binary.cursor + BIT_LENGTH + BIT_LENGTH * 2 >= png->binary.size) break;

        char bfinal[BIT_LENGTH];
        writeBuffer(png->binary.data, bfinal, png->binary.cursor, png->binary.cursor + BIT_LENGTH);
        png->binary.cursor += BIT_LENGTH;

        huffman.bfinal = binaryToInt(bfinal, BIT_LENGTH, true);

        char btype[BIT_LENGTH * 2];
        writeBuffer(png->binary.data, btype, png->binary.cursor, png->binary.cursor + BIT_LENGTH * 2);

        png->binary.cursor += BIT_LENGTH * 2;
        huffman.btype = binaryToInt(btype, BIT_LENGTH * 2, true);

        handleBtype(
            png,
            &huffman
        );

        if (huffman.bfinal == 1) break;
    }
}
