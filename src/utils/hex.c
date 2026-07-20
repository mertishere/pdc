#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "utils.h"

#define HEX_PRINT_AMOUNT 2

void hexReverse(char buffer[], size_t size) {
    for(size_t i = 0; i < (size / 2); i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[i + (size / 2)];
        buffer[i + (size / 2)] = tmp;
    }
}

size_t getHexDump(
    char buffer[],
    size_t buffer_size,

    char hex[],
    size_t hex_size,

    char *file_path
) {
    FILE *file = fopen(file_path, "rb");
    if(file == NULL) return 0;

    int read_buffer_size = 1024;
    char read_buffer[read_buffer_size];

    size_t bytes_total = 0;
    size_t bytes_read = 0;
    while((bytes_read = fread(read_buffer, 1, read_buffer_size - 1, file)) > 0) {
        if(bytes_total + bytes_read > buffer_size) return 0;

        for(size_t i = 0; i < bytes_read; i++) {
            buffer[i + bytes_total] = read_buffer[i];
        }

        bytes_total += bytes_read;
    }

    fclose(file);
    hexDump(buffer, bytes_total - 1, hex, hex_size);
    
    return bytes_total * 2; // return size of hex
}

void hexDump(char buffer[], size_t buffer_size, char hex[], size_t hex_size) {
    char str_storer[HEX_PRINT_AMOUNT];
    char hex_storer[HEX_PRINT_AMOUNT * 2];

    size_t index = 0;
    while (index < buffer_size) {
        if(index * HEX_PRINT_AMOUNT > buffer_size) break;

        for(int j = 0; j < HEX_PRINT_AMOUNT; j++) {
            if(index * HEX_PRINT_AMOUNT + j > buffer_size) break;

            str_storer[j] = buffer[index * HEX_PRINT_AMOUNT + j];
        }

        hexMultiple(
            str_storer,
            HEX_PRINT_AMOUNT,
            hex_storer,
            HEX_PRINT_AMOUNT * 2
        );

        for(int k = 0; k < (HEX_PRINT_AMOUNT * 2); k++) {
            size_t i = index * (HEX_PRINT_AMOUNT * HEX_PRINT_AMOUNT) + k;
            if(i >= hex_size) return;
            hex[i] = hex_storer[k];
        }

        index++;
    }
}

void hexMultiple(char buffer[], size_t buffer_size, char hex[], size_t hex_size) {
    char hex_storer[2];
    for (size_t i = 0; i < buffer_size; i++) {
        unsigned char byte = (unsigned char)buffer[i];
        intToHex(byte, hex_storer, 2);

        if((i * 2 + 1) > hex_size) return;

        hex[i * 2] = hex_storer[0];
        hex[i * 2 + 1] = hex_storer[1];
    }
}
