#include <stdlib.h>

void writeBuffer(char src[], char out[], size_t start, size_t end) {
    int k = 0;
    for(int i = start; i < end; i++) {
        out[k] = src[i];
        k++;
    }
}

void writeBufferOffset(char src[], char out[], size_t start, size_t end, size_t offset) {
    int k = offset;
    for(int i = start; i < end; i++) {
        out[k] = src[i];
        k++;
    }
}