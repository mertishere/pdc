#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

void removeWhitespace(char buffer[], size_t size) {
    size_t index = 0;
    size_t ws_index = 0;
    while(index < size) {
        if(buffer[index] == ' ') {
            ws_index = index;
            while(buffer[index] == ' ' && index < size) index++;
            if(index == size) {
                buffer[ws_index] = '\0';
                break;
            }
            for(size_t j = index; j < size; j++) {
                buffer[ws_index + (j - index)] = buffer[j];
            }
            index = ws_index;
            continue;
        }

        index++;
    }
}

void reverseString(char buffer[], size_t size) {
    int left = 0;
    int right = size - 1;

    while (left < right) {
        char tmp = buffer[left];
        buffer[left] = buffer[right];
        buffer[right] = tmp;

        left++;
        right--;
    }
}

bool cmpInts(
    int c1[],
    int len1,
    int c2[],
    int len2
) {
    if (len1 != len2) return false;

    for (int i = 0; i < len1; i++) {
        if (c1[i] != c2[i]) return false;
    }
    
    return true;
}