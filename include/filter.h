#ifndef FILTER_H
#define FILTER_H

#include "png.h"

extern void getFilter(
    Pixel pixels[],
    size_t *pixels_len, 
    
    int uncompressed_data[], 
    size_t uncompressed_data_len, 
    PNG *png
);

#endif