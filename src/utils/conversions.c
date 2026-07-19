#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "constants.h"
#include "utils.h"

int powOfInt(int byte) {
    int p = 0;
    
    int x = 1 << p;
    while(x < byte) {
        p++;
        x = 1 << p;
    }

    if(x != byte) {
        p--;
    }

    return p;
}

// to BINARY
void intToBinary(int byte, char *buffer, size_t size) {
    for(size_t i = 0; i < size; i++) {
        buffer[i] = '0';
    }

    int stored_byte = byte;
    while(stored_byte > 0) {
        int p = powOfInt(stored_byte);

        int x = size - p - 1;
        if(x < 0) return;

        buffer[x] = '1';
        stored_byte -= 1 << p;
    }

    return;
}

void hexToBinary(
    char *hex,
    size_t hex_size,

    char *buffer,
    size_t buffer_size,
    bool lsb
) {
    for(size_t i = 0; i < buffer_size; i++) {
        buffer[i] = '0';
    }

    int buffer_index = 0;
    size_t index = 0;

    char hex_storer[8];
    int hex_index = 0;

    while(index < hex_size) {
        char h = hex[index];
        int hi = 0;
        for(int i = 0; i < 16; i++) {
            if(h == HEX_KEYS[i]) {
                hi = i;
                break;
            }
        }

        char *value = HEX_VALUES[hi];
        for(int i = 0; i < 4; i++) {
            hex_storer[hex_index + i] = value[i];
        }

        if(index % 2 != 0) {
            if(lsb) {
                reverseString(hex_storer, 8);
            }
            for(int i = 0; i < 8; i++) {
                buffer[buffer_index + i] = hex_storer[i];
            }
            buffer_index += 8;
            hex_index = 0;
        } else {
            hex_index += 4;
        }

        index += 1;
    }
}

void intToHex(int byte, char *buffer, size_t size) {
    for(size_t i = 0; i < size; i++) {
        buffer[i] = '0';
    }

    int divisor = 16;
    int multiple = 0;
    int last_stored_quotient = byte;

    int index = 0;
    while(last_stored_quotient != 0) {
        multiple = 0;
        while(divisor * (multiple + 1) <= last_stored_quotient) {
            multiple += 1;
        }
        int remainder = last_stored_quotient - divisor * multiple;
        buffer[size - index - 1] = remainder > 9 ? HEX_LETTERS[remainder - 10] : remainder + '0';

        index++;
        last_stored_quotient = multiple;
    }

    return;
}


void binaryToHex(char *binary, char *buffer, size_t size) {
    int i = binaryToInt(binary, false, strlen(binary));
    intToHex(i, buffer, size);
}

int hexToInt(char *hex, size_t size) {
    int final = 0;

    int index = size - 1;
    while(index >= 0) {
        int h = hex[index];
        int hi = 0;
        for(int i = 0; i < 16; i++) {
            if(h == HEX_KEYS[i]) {
                hi = i;
                break;
            }
        }

        final += hi * pow(16, (size - 1) - index);
        index--;
    }

    return final;
}

int binaryToInt(char *binary, size_t size, bool lsb) {
    int final = 0;
    if(size == 0) return final;

    char cbinary[size];
    for(size_t i = 0; i < size; i++) {
        cbinary[i] = binary[i];
    }

    if(lsb == true) {
        reverseString(cbinary, size);
    }

    int index = size;
    while (index > 0) {
        int bin = size - index;
        int parsed_int = cbinary[index - 1] - '0';
        int parsed_binary = pow(2, bin);
        final += parsed_int * parsed_binary;

        index -= 1;
    }
    
    return final;
}
