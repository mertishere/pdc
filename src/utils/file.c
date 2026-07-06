#include <stdio.h>
#include <stdlib.h>

size_t getFileSize(char *path) {
    FILE *f = fopen(path, "r");
    fseek(f, 0L, SEEK_END);
    size_t size = (size_t)ftell(f);
    fclose(f);
    return size;
}