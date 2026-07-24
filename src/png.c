#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "utils.h"
#include "constants.h"
#include "algorithms.h"

void getIhdr(PNG *png) {
    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char ihdr_size_slice[BYTE_LENGTH];
    writeBuffer(png->hex.data, ihdr_size_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    int ihdr_size = hexToInt(ihdr_size_slice, BYTE_LENGTH);
    png->ihdr.size = (size_t)ihdr_size;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char ihdr_header_slice[BYTE_LENGTH + 1];
    writeBuffer(png->hex.data, ihdr_header_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    ihdr_header_slice[BYTE_LENGTH] = '\0';
    
    if (strcmp(ihdr_header_slice, IHDR_SIG) != 0) {
        printf("Not correct IHDR header!");
        return;
    }

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char ihdr_png_width[BYTE_LENGTH];
    writeBuffer(png->hex.data, ihdr_png_width, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char ihdr_png_height[BYTE_LENGTH];
    writeBuffer(png->hex.data, ihdr_png_height, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char ihdr_png_bbp[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, ihdr_png_bbp, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;

    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char ihdr_png_ct[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, ihdr_png_ct, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;

    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char ihdr_png_cm[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, ihdr_png_cm, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;

    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char ihdr_png_fm[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, ihdr_png_fm, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;

    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char ihdr_png_ii[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, ihdr_png_ii, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;

    // char ihdr_png_crc[BYTE_LENGTH];
    // writeBuffer(png->hex.data, ihdr_png_crc, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    // define IHDR
    png->ihdr.width = (size_t)hexToInt(ihdr_png_width, BYTE_LENGTH);
    png->ihdr.height = (size_t)hexToInt(ihdr_png_height, BYTE_LENGTH);
    png->ihdr.bits_per_pixel = hexToInt(ihdr_png_bbp, BIT_LENGTH * 2);
    png->ihdr.color_type = hexToInt(ihdr_png_ct, BIT_LENGTH * 2);
    png->ihdr.compression_method = hexToInt(ihdr_png_cm, BIT_LENGTH * 2);
    png->ihdr.filter_method = hexToInt(ihdr_png_fm, BIT_LENGTH * 2);
    png->ihdr.interlaced = hexToInt(ihdr_png_ii, BIT_LENGTH * 2) == 1;
    // png->ihdr.crc = ihdr_png_crc;
}

void getPlte(PNG *png) {
    if (png->ihdr.color_type == 0 || png->ihdr.color_type == 4) return;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char plte_size_slice[BYTE_LENGTH];
    writeBuffer(png->hex.data, plte_size_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
    char plte_header[BYTE_LENGTH + 1];
    writeBuffer(png->hex.data, plte_header, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;
    plte_header[BYTE_LENGTH] = '\0';

    if(strcmp(plte_header, PLTE_SIG) != 0) {
        png->hex.cursor -= BYTE_LENGTH * 2;
        return;
    }

    png->plte.size = (size_t)hexToInt(plte_size_slice, BYTE_LENGTH);

    if((png->hex.cursor + BYTE_LENGTH * 3) > png->hex.size) return;
    char idat_header[BYTE_LENGTH + 1];
    writeBuffer(png->hex.data, idat_header, png->hex.cursor + BYTE_LENGTH * 2, png->hex.cursor + BYTE_LENGTH * 3);
    idat_header[BYTE_LENGTH] = '\0';

    size_t rgb_size = 0;

    size_t plte_size = 0;
    while (plte_size < (png->plte.size * 2)) {
        if(png->hex.size < png->hex.cursor) break;
        char red_palette_slice[(BYTE_LENGTH / 2)];
        char green_palette_slice[(BYTE_LENGTH / 2)];
        char blue_palette_slice[(BYTE_LENGTH / 2)];
        
        if((png->hex.cursor + BYTE_LENGTH / 2) > png->hex.size) return;
        writeBuffer(png->hex.data, red_palette_slice, png->hex.cursor, png->hex.cursor + (BYTE_LENGTH / 2));
        png->hex.cursor += (BYTE_LENGTH / 2);

        if((png->hex.cursor + BYTE_LENGTH / 2) > png->hex.size) return;
        writeBuffer(png->hex.data, green_palette_slice, png->hex.cursor, png->hex.cursor + (BYTE_LENGTH / 2));
        png->hex.cursor += (BYTE_LENGTH / 2);
        
        if((png->hex.cursor + BYTE_LENGTH / 2) > png->hex.size) return;
        writeBuffer(png->hex.data, blue_palette_slice, png->hex.cursor, png->hex.cursor + (BYTE_LENGTH / 2));
        png->hex.cursor += (BYTE_LENGTH / 2);

        plte_size += (BYTE_LENGTH / 2) * 3;

        int red_palette = hexToInt(red_palette_slice, (BYTE_LENGTH / 2));
        int green_palette = hexToInt(green_palette_slice, (BYTE_LENGTH / 2));
        int blue_palette = hexToInt(blue_palette_slice, (BYTE_LENGTH / 2));

        png->plte.rgbs[rgb_size].r = red_palette;
        png->plte.rgbs[rgb_size].g = green_palette;
        png->plte.rgbs[rgb_size].b = blue_palette;
        png->plte.rgbs[rgb_size].a = 255;
        rgb_size++;

        if((png->hex.cursor + BYTE_LENGTH * 2) > png->hex.size) return;

        writeBuffer(png->hex.data, idat_header, png->hex.cursor + BYTE_LENGTH * 2, png->hex.cursor + BYTE_LENGTH * 3);
        idat_header[BYTE_LENGTH] = '\0';
    }

    png->hex.cursor += BYTE_LENGTH;
}

void getAncillary(
    PNG *png,
    size_t *ap
) {
    size_t ancillary_position = *ap;

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        header, 
        png->hex.cursor + BYTE_LENGTH,
        png->hex.cursor + BYTE_LENGTH * 2
    );
    
    header[BYTE_LENGTH] = '\0';

    while (
        strcmp(header, IDAT_SIG) != 0 
        && 
        strcmp(header, PLTE_SIG) != 0 
        && 
        strcmp(header, IEND_SIG) != 0 
        && 
        strcmp(header, IHDR_SIG) != 0 
        && 
        png->hex.cursor < png->hex.size
    ) 
    {
        if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

        char ancillary_header_size_slice[BYTE_LENGTH];
        writeBuffer(png->hex.data, ancillary_header_size_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
        png->hex.cursor += BYTE_LENGTH;

        int ancillary_header_size = hexToInt(ancillary_header_size_slice, BYTE_LENGTH);
        if(ancillary_header_size < 0) break;

        if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

        char ancillary_header_slice[BYTE_LENGTH + 1];
        writeBuffer(png->hex.data, ancillary_header_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
        png->hex.cursor += BYTE_LENGTH;
        ancillary_header_slice[BYTE_LENGTH] = '\0';

        strcpy(png->ancillaries[ancillary_position].signature, ancillary_header_slice);
        png->ancillaries[ancillary_position].size = ancillary_header_size;

        png->ancillaries[ancillary_position].data = malloc(ancillary_header_size * 2 * sizeof(char));
        writeBuffer(
            png->hex.data,
            png->ancillaries[ancillary_position].data,
            png->hex.cursor,
            png->hex.cursor + ancillary_header_size * 2
        );

        png->hex.cursor += ancillary_header_size * 2;
        png->hex.cursor += BYTE_LENGTH; // crc

        if((png->hex.cursor + BYTE_LENGTH * 2) > png->hex.size) return;

        writeBuffer(
            png->hex.data,
            header, 
            png->hex.cursor + BYTE_LENGTH,
            png->hex.cursor + BYTE_LENGTH * 2
        );
        
        header[BYTE_LENGTH] = '\0';
        ancillary_position++;
    }

    *ap = ancillary_position;
}

void getIdat(
    PNG *png,
    int idat_index
) {
    png->idats[idat_index].size = 0;
    png->idats[idat_index].data = NULL;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

    char idat_size_slice[BYTE_LENGTH];
    writeBuffer(png->hex.data, idat_size_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;
    int idat_size = hexToInt(idat_size_slice, BYTE_LENGTH);

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

    char idat_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        idat_header_slice, 
        png->hex.cursor, 
        png->hex.cursor + BYTE_LENGTH
    );
    png->hex.cursor += BYTE_LENGTH;
    idat_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(idat_header_slice, IDAT_SIG) != 0) {
        png->hex.cursor += BYTE_LENGTH * 3; // skipping header, size, and crc
        png->hex.cursor += idat_size;
        return;
    }
    png->idats[idat_index].size = (size_t)idat_size;

    if (idat_index > 0) {
        if((png->hex.cursor + idat_size * 2) > png->hex.size) return;
        
        png->idats[idat_index].data = malloc((idat_size * 2) * sizeof(char));
        writeBuffer(
            png->hex.data,
            png->idats[idat_index].data,
            png->hex.cursor,
            png->hex.cursor + (idat_size * 2)
        );

        png->hex.cursor += (idat_size * 2); // crc
        if((png->hex.cursor + BYTE_LENGTH * 4) > png->hex.size) return;

        char iend_header[BYTE_LENGTH + 1];
        writeBuffer(
            png->hex.data,
            iend_header,
            png->hex.cursor + BYTE_LENGTH * 3,
            png->hex.cursor + BYTE_LENGTH * 4
        );
        iend_header[BYTE_LENGTH] = '\0';

        int adler_zlib_checksum = 0;
        if (strcmp(iend_header, IEND_SIG) == 0) {
            if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

            char adler_zlib_checksum_slice[BYTE_LENGTH];
            writeBuffer(
                png->hex.data,
                adler_zlib_checksum_slice,
                png->hex.cursor,
                png->hex.cursor + BYTE_LENGTH
            );
            png->hex.cursor += BYTE_LENGTH;
            
            adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
        }

        if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
        char idat_png_crc[BYTE_LENGTH + 1];
        writeBuffer(
            png->hex.data,
            idat_png_crc,
            png->hex.cursor,
            png->hex.cursor + BYTE_LENGTH
        );
        png->hex.cursor += BYTE_LENGTH;
            
        // define IDAT
        png->idats[idat_index].compression_info = 0;
        png->idats[idat_index].compression_method = 0;
        png->idats[idat_index].zlib_fcheck_value = 0;
        png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
        // png->idats[idat_index].crc = idat_png_crc;
        return;
    }

    if((png->hex.cursor + BIT_LENGTH) > png->hex.size) return;
    char deflate_compression_info[BIT_LENGTH];
    deflate_compression_info[0] = png->hex.data[png->hex.cursor];
    png->hex.cursor += BIT_LENGTH;
    
    if((png->hex.cursor + BIT_LENGTH) > png->hex.size) return;
    char deflate_compression_method[BIT_LENGTH];
    deflate_compression_method[0] = png->hex.data[png->hex.cursor];
    png->hex.cursor += BIT_LENGTH;
    
    if((png->hex.cursor + BIT_LENGTH * 2) > png->hex.size) return;
    char zlib_fcheck_value[BIT_LENGTH * 2];
    writeBuffer(png->hex.data, zlib_fcheck_value, png->hex.cursor, png->hex.cursor + BIT_LENGTH * 2);
    png->hex.cursor += BIT_LENGTH * 2;
    
    // validate the zlib header
    char validation_value_hex[BIT_LENGTH * 4 + 1];
    validation_value_hex[0] = deflate_compression_info[0];
    validation_value_hex[1] = deflate_compression_method[0];
    validation_value_hex[2] = zlib_fcheck_value[0];
    validation_value_hex[3] = zlib_fcheck_value[1];
    validation_value_hex[BIT_LENGTH * 4] = '\0';

    int validation_value = hexToInt(validation_value_hex, BIT_LENGTH * 4);
    if (validation_value % 31 != 0) {
        printf("invalid zlib header\n");
        exit(EXIT_FAILURE);
        return;
    }

    if((png->hex.cursor + BYTE_LENGTH * 3) > png->hex.size) return;
    char iend_header[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        iend_header, 
        png->hex.cursor + BYTE_LENGTH * 2, 
        png->hex.cursor + BYTE_LENGTH * 3
    );
    iend_header[BYTE_LENGTH] = '\0';

    int adler_zlib_checksum = 0;
    if (strcmp(iend_header, IEND_SIG) == 0) {
        if((png->hex.cursor + idat_size * 2 - 12) > png->hex.size) return;

        png->idats[idat_index].data = malloc((idat_size * 2 - 12) * sizeof(char));
        writeBuffer(
            png->hex.data,
            png->idats[idat_index].data,
            png->hex.cursor,
            png->hex.cursor + idat_size * 2 - 12
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2), ADLER32 (8)
        png->hex.cursor += (idat_size * 2) - 12;

        if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;
        char adler_zlib_checksum_slice[BYTE_LENGTH];
        writeBuffer(
            png->hex.data, 
            adler_zlib_checksum_slice, 
            png->hex.cursor, 
            png->hex.cursor + BYTE_LENGTH
        );
        png->hex.cursor += BYTE_LENGTH;
        adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
    } else {
        if((png->hex.cursor + idat_size * 2 - 4) > png->hex.size) return;

        png->idats[idat_index].data = malloc((idat_size * 2 - 4) * sizeof(char));
        writeBuffer(
            png->hex.data,
            png->idats[idat_index].data,
            png->hex.cursor,
            png->hex.cursor + idat_size * 2 - 4
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2)
        png->hex.cursor += (idat_size * 2) - 4;
    }

    // char idat_png_crc[BYTE_LENGTH];
    // writeBuffer(png->hex.data, idat_png_crc, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    // define IDAT
    png->idats[idat_index].compression_info = hexToInt(deflate_compression_info, BIT_LENGTH);
    png->idats[idat_index].compression_method = hexToInt(deflate_compression_method, BIT_LENGTH);
    png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
    png->idats[idat_index].zlib_fcheck_value = validation_value;
    // png->idats[idat_index].crc = idat_png_crc;
}

void getIend(PNG *png) {
    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

    char iend_size_slice[BYTE_LENGTH];
    writeBuffer(png->hex.data, iend_size_slice, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;

    png->iend.size = (size_t)hexToInt(iend_size_slice, BYTE_LENGTH);

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

    char iend_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        iend_header_slice, 
        png->hex.cursor, 
        png->hex.cursor + BYTE_LENGTH
    );
    png->hex.cursor += BYTE_LENGTH;
    iend_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(iend_header_slice, IEND_SIG) != 0) {
        printf("Not correct IEND header!");
        return;
    }

    png->hex.cursor += png->iend.size;

    if((png->hex.cursor + BYTE_LENGTH) > png->hex.size) return;

    char iend_png_src[BYTE_LENGTH];
    writeBuffer(png->hex.data, iend_png_src, png->hex.cursor, png->hex.cursor + BYTE_LENGTH);
    png->hex.cursor += BYTE_LENGTH;
}

int countIdats(PNG *png) {
    int c = 0; // *counter;

    size_t hex_position = 0;

    // skip PNG header
    hex_position += BYTE_LENGTH * 2;

    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        size, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        header, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    header[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    while(strcmp(header, IEND_SIG) != 0) {
        if(strcmp(header, IDAT_SIG) == 0) c++;
        int chunk_size = hexToInt(size, BYTE_LENGTH);

        // A negative (i.e. >= 2^31, malformed) length would wrap when cast to
        // size_t and could leave hex_position un-advanced, spinning this loop.
        if (chunk_size < 0) break;    
        hex_position += (size_t)chunk_size * 2;
        hex_position += BYTE_LENGTH; // crc

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            size, 
            hex_position,
            hex_position + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            header, 
            hex_position, 
            hex_position + BYTE_LENGTH
        );
        header[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;
    }

    return c;
}

int countAncillaries(PNG *png) {
    int c = 0; // *counter;

    size_t hex_position = 0;

    // skip PNG header
    hex_position += BYTE_LENGTH * 2;

    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        size, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        header, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    header[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    while(strcmp(header, IEND_SIG) != 0) {
        bool not_idat = strcmp(header, IDAT_SIG) != 0;
        bool not_iend = strcmp(header, IEND_SIG) != 0;
        bool not_plte = strcmp(header, PLTE_SIG) != 0;
        bool not_ihdr = strcmp(header, IHDR_SIG) != 0;
        if(not_plte & not_ihdr & not_idat & not_iend) c++;

        int chunk_size = hexToInt(size, BYTE_LENGTH);

        // A negative (i.e. >= 2^31, malformed) length would wrap when cast to
        // size_t and could leave hex_position un-advanced, spinning this loop.
        if (chunk_size < 0) break;    
        hex_position += (size_t)chunk_size * 2;
        hex_position += BYTE_LENGTH; // crc

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            size, 
            hex_position,
            hex_position + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            header, 
            hex_position, 
            hex_position + BYTE_LENGTH
        );
        header[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;
    }

    return c;
}

int countRgbs(PNG *png) {
    int c = 0; // *counter;

    size_t hex_position = 0;

    // skip PNG header
    hex_position += BYTE_LENGTH * 2;
    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        size, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > png->hex.size) {
        return c;
    }

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        header, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    header[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    while(strcmp(header, IEND_SIG) != 0) {
        int chunk_size = hexToInt(size, BYTE_LENGTH);

        // A negative (i.e. >= 2^31, malformed) length would wrap when cast to
        // size_t and could leave hex_position un-advanced, spinning this loop.
        if (chunk_size < 0) break;
        hex_position += (size_t)chunk_size * 2;
        hex_position += BYTE_LENGTH; // crc

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            size, 
            hex_position,
            hex_position + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH) > png->hex.size) {
            return c;
        }

        writeBuffer(
            png->hex.data, 
            header, 
            hex_position, 
            hex_position + BYTE_LENGTH
        );
        header[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if(strcmp(header, PLTE_SIG) != 0) continue;

        c = chunk_size / 3;
        return c;
    }

    return c;
}

void getPng(PNG *png)  {
    if((png->hex.cursor + BYTE_LENGTH * 2) > png->hex.size) return;

    char png_signature[BYTE_LENGTH * 2 + 1];
    writeBuffer(
        png->hex.data,
        png_signature,
        png->hex.cursor,
        png->hex.cursor + BYTE_LENGTH * 2
    );
    png->hex.cursor += BYTE_LENGTH * 2;
    png_signature[BYTE_LENGTH * 2] = '\0';

    // verify PNG signature
    if(strcmp(png_signature, PNG_SIG) != 0) {
        printf("Invalid PNG signature");
        return;
    }

    size_t ancillary_position = 0;
    getIhdr(png);
    getAncillary(png, &ancillary_position);

    getPlte(png);
    getAncillary(png, &ancillary_position);

    if((png->hex.cursor + BYTE_LENGTH * 2) > png->hex.size) return;

    char next_signature[BYTE_LENGTH + 1];
    writeBuffer(
        png->hex.data, 
        next_signature, 
        png->hex.cursor + BYTE_LENGTH, 
        png->hex.cursor + BYTE_LENGTH * 2
    );

    next_signature[BYTE_LENGTH] = '\0';
    int idat_index = 0;

    while(strcmp(next_signature, IDAT_SIG) == 0) {
        if(png->hex.cursor >= png->hex.size) {
            printf("Exceeding hex size.\n");
            exit(EXIT_FAILURE);
        }

        getIdat(png, idat_index);
        idat_index++;

        if((png->hex.cursor + BYTE_LENGTH * 2) > png->hex.size) return;

        writeBuffer(
            png->hex.data,
            next_signature,
            png->hex.cursor + BYTE_LENGTH,
            png->hex.cursor + BYTE_LENGTH * 2
        );
        next_signature[BYTE_LENGTH] = '\0';
    }

    getAncillary(png, &ancillary_position);
    getIend(png);
}

PNG parsePng(char *path) {
    PNG png = {};
    size_t size = getFileSize(path);

    png.path = path;
    png.size = size;

    png.buffer.size = png.size + BIT_LENGTH;
    png.buffer.data = malloc(png.buffer.size * sizeof(char));

    png.hex.size = png.size * 2 + BIT_LENGTH;
    png.hex.data = malloc(png.hex.size * sizeof(char));

    size_t hex_len = getHexDump(
        png.buffer.data,
        png.buffer.size,

        png.hex.data,
        png.hex.size,
        path
    );

    png.hex.size = hex_len;

    png.ancillaries_amount = countAncillaries(&png);
    png.ancillaries = malloc(png.ancillaries_amount * sizeof(ANCI));
    if(png.ancillaries == NULL) {
        printf("Allocation failed (ancillaries).\n");
        exit(EXIT_FAILURE);
    }

    png.idats_amount = countIdats(&png);
    png.idats = malloc(png.idats_amount * sizeof(IDAT));
    if(png.idats == NULL) {
        printf("Allocation failed (idats).\n");
        exit(EXIT_FAILURE);
    }

    png.rgbs_amount = countRgbs(&png);
    png.plte.rgbs = malloc(png.rgbs_amount * sizeof(RGB));
    if(png.plte.rgbs == NULL) {
        printf("Allocation failed (rgbs).\n");
        exit(EXIT_FAILURE);
    }

    getPng(&png);

    size_t idats_size = 0;
    for(int i = 0; i < png.idats_amount; i++) {
        idats_size += png.idats[i].size;
    }

    // - 4 * 4 (because of the first IDAT)
    png.binary.size = idats_size * 8 - (4 * 4);
    png.binary.cursor = 0;
    png.binary.data = malloc(png.binary.size * sizeof(char));
    if(png.binary.data == NULL) {
        printf("Allocation failed (binary).\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < png.idats_amount; i++) {
        size_t idat_data_size = png.idats[i].size * 2;
        if(i == 0) idat_data_size -= 4;

        size_t binary_idat_data_size = idat_data_size * 4;
        char *binary_idat_data = malloc(binary_idat_data_size * sizeof(char));
        if(binary_idat_data == NULL) {
            printf("Allocation failed (idat).\n");
            exit(EXIT_FAILURE);
        }

        hexToBinary(
            png.idats[i].data,
            idat_data_size,

            binary_idat_data,
            binary_idat_data_size,
            true
        );
        
        writeBufferOffset(
            binary_idat_data,
            png.binary.data,

            0,
            binary_idat_data_size,

            png.binary.cursor // offset
        );
        png.binary.cursor += binary_idat_data_size;

        free(binary_idat_data);
    }

    int size_multiplicator = 0;
    switch (png.ihdr.color_type) {
        case 0:
            size_multiplicator = 1;
            break;

        case 2:
            size_multiplicator = 3;
            break;

        case 4:
            size_multiplicator = 2;
            break;

        case 6:
            size_multiplicator = 4;
            break;

        default:
            break;
    }

    png.raw.size = png.ihdr.height * (1 + png.ihdr.width * size_multiplicator);
    png.raw.data = malloc(png.raw.size * sizeof(int));
    if(png.raw.data == NULL) {
        printf("Allocation failed (uncompressed).\n");
        exit(EXIT_FAILURE);
    }
    png.raw.cursor = 0;
    getHuffman(&png);

    png.pixels.size = png.ihdr.height * (1 + png.ihdr.width * size_multiplicator);
    png.pixels.data = malloc(png.pixels.size * sizeof(Pixel));
    if(png.pixels.data == NULL) {
        printf("Allocation failed (pixels).\n");
        exit(EXIT_FAILURE);
    }

    png.pixels.cursor = 0;
    getFilter(&png);

    return png;
}

void freePng(PNG *png) {
    free(png->buffer.data);
    free(png->hex.data);
    free(png->binary.data);
    free(png->raw.data);
    free(png->pixels.data);
    for(int i = 0; i < png->idats_amount; i++) {
        free(png->idats[i].data);
    }
    free(png->idats);
    free(png->plte.rgbs);
    for(int i = 0; i < png->ancillaries_amount; i++) {
        free(png->ancillaries[i].data);
    }
    free(png->ancillaries);
} 