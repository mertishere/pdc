#include <stdio.h>
#include <stdbool.h>

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