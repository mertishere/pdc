#include <stdio.h>
#include <stdlib.h>

void removeWhitespace(char buffer[], size_t size) {
    int index = 0;
    int ws_index = 0;
    while(index < size) {
        if(buffer[index] == ' ') {
            ws_index = index;
            while(buffer[index] == ' ' && index < size) index++;
            if(index == size) {
                buffer[ws_index] = '\0';
                break;
            }
            for(int j = index; j < size; j++) {
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