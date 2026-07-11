#include <stdlib.h>

void writeBuffer(char src[], char out[], size_t start, size_t end) {
    size_t k = 0;
    for(size_t i = start; i < end; i++) {
        out[k] = src[i];
        k++;
    }
}

void writeBufferOffset(char src[], char out[], size_t start, size_t end, size_t offset) {
    size_t k = offset;
    for(size_t i = start; i < end; i++) {
        out[k] = src[i];
        k++;
    }
}