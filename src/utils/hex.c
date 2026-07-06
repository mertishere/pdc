#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "utils.h"

void hexReverse(char buffer[], size_t size) {
    for(int i = 0; i < (size / 2); i++) {
        char tmp = buffer[i];
        buffer[i] = buffer[i + (size / 2)];
        buffer[i + (size / 2)] = tmp;
    }
}

void getHexDump(char buffer[], size_t buffer_size, char hex[], size_t hex_size, char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if(file == NULL) return;

    int read_buffer_size = 1024;
    char read_buffer[read_buffer_size];

    size_t bytes_total = 0;
    size_t bytes_read = 0;
    while((bytes_read = fread(read_buffer, 1, read_buffer_size - 1, file)) > 0) {
        if(bytes_total + bytes_read > buffer_size) return;

        for(int i = 0; i < bytes_read; i++) {
            buffer[i + bytes_total] = read_buffer[i];
        }

        bytes_total += bytes_read;
    }

    buffer[bytes_total] = '\0';
    fclose(file);
    hexDump(buffer, bytes_total, hex, hex_size);
}


void hexDump(char bytes[], size_t bytes_size, char buffer[], size_t buffer_size) {
    char str[HEX_PRINT_AMOUNT];
    char hex[HEX_PRINT_AMOUNT * 2];

    int index = 0;
    while (index < bytes_size) {
        if(index * HEX_PRINT_AMOUNT > bytes_size) {
            break;
        }
        for(int j = 0; j < HEX_PRINT_AMOUNT; j++) {
            if(index * HEX_PRINT_AMOUNT + j > bytes_size) {
                break;
            }
            str[j] = bytes[index * HEX_PRINT_AMOUNT + j];
        }

        hexMultiple(
            str,
            HEX_PRINT_AMOUNT,
            hex,
            HEX_PRINT_AMOUNT * 2
        );

        // hexReverse(hex, HEX_PRINT_AMOUNT * 2);
        for(int k = 0; k < (HEX_PRINT_AMOUNT * 2); k++) {
            buffer[index * (HEX_PRINT_AMOUNT * HEX_PRINT_AMOUNT) + k] = hex[k];
        }

        index++;
    }

    buffer[index * (HEX_PRINT_AMOUNT * HEX_PRINT_AMOUNT)] = '\0';
}

void hexMultiple(char bytes[], size_t bytes_size, char buffer[], size_t buffer_size) {
    char hex[2];
    for (int i = 0; i < bytes_size; i++) {
        unsigned char byte = (unsigned char)bytes[i];
        intToHex(byte, hex, 2);

        buffer[i * 2] = hex[0];
        buffer[i * 2 + 1] = hex[1];
    }
}
