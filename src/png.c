#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "utils.h"
#include "constants.h"

#define BIT_LENGTH 1
#define BYTE_LENGTH 8

void getIhdr(
    PNG *png,
    char hex[],
    size_t hex_size,
    size_t *hp
) {
    size_t hex_position = *hp;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char ihdr_size_slice[BYTE_LENGTH];
    writeBuffer(hex, ihdr_size_slice, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    int ihdr_size = hexToInt(ihdr_size_slice, BYTE_LENGTH);
    png->ihdr.size = (size_t)ihdr_size;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char ihdr_header_slice[BYTE_LENGTH + 1];
    writeBuffer(hex, ihdr_header_slice, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    ihdr_header_slice[BYTE_LENGTH] = '\0';
    
    if (strcmp(ihdr_header_slice, IHDR_SIG) != 0) {
        printf("not correct IHDR header!");
        return;
    }

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char ihdr_png_width[BYTE_LENGTH];
    writeBuffer(hex, ihdr_png_width, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char ihdr_png_height[BYTE_LENGTH];
    writeBuffer(hex, ihdr_png_height, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char ihdr_png_bbp[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_bbp, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;

    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char ihdr_png_ct[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_ct, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;

    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char ihdr_png_cm[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_cm, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;

    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char ihdr_png_fm[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_fm, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;

    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char ihdr_png_ii[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_ii, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;

    // char ihdr_png_crc[BYTE_LENGTH];
    // writeBuffer(hex, ihdr_png_crc, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    // define IHDR
    png->ihdr.width = (size_t)hexToInt(ihdr_png_width, BYTE_LENGTH);
    png->ihdr.height = (size_t)hexToInt(ihdr_png_height, BYTE_LENGTH);
    png->ihdr.bits_per_pixel = hexToInt(ihdr_png_bbp, BIT_LENGTH * 2);
    png->ihdr.color_type = hexToInt(ihdr_png_ct, BIT_LENGTH * 2);
    png->ihdr.compression_method = hexToInt(ihdr_png_cm, BIT_LENGTH * 2);
    png->ihdr.filter_method = hexToInt(ihdr_png_fm, BIT_LENGTH * 2);
    png->ihdr.interlaced = hexToInt(ihdr_png_ii, BIT_LENGTH * 2) == 1;
    // png->ihdr.crc = ihdr_png_crc;

    *hp = hex_position;
}

void getPlte(
    PNG *png,
    char hex[],
    size_t hex_size,
    size_t *hp
) {
    size_t hex_position = *hp;

    if (png->ihdr.color_type == 0 || png->ihdr.color_type == 4) return;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char plte_size_slice[BYTE_LENGTH];
    writeBuffer(hex, plte_size_slice, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    char plte_header[BYTE_LENGTH + 1];
    writeBuffer(hex, plte_header, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;
    plte_header[BYTE_LENGTH] = '\0';

    if(strcmp(plte_header, PLTE_SIG) != 0) {
        hex_position -= BYTE_LENGTH * 2;
        return;
    }

    png->plte.size = (size_t)hexToInt(plte_size_slice, BYTE_LENGTH);

    if((hex_position + BYTE_LENGTH * 3) > hex_size) return;
    char idat_header[BYTE_LENGTH + 1];
    writeBuffer(hex, idat_header, hex_position + BYTE_LENGTH * 2, hex_position + BYTE_LENGTH * 3);
    idat_header[BYTE_LENGTH] = '\0';

    size_t rgb_size = 0;

    size_t plte_size = 0;
    while (plte_size < (png->plte.size * 2)) {
        if(hex_size < hex_position) break;
        char red_palette_slice[(BYTE_LENGTH / 2)];
        char green_palette_slice[(BYTE_LENGTH / 2)];
        char blue_palette_slice[(BYTE_LENGTH / 2)];
        
        if((hex_position + BYTE_LENGTH / 2) > hex_size) return;
        writeBuffer(hex, red_palette_slice, hex_position, hex_position + (BYTE_LENGTH / 2));
        hex_position += (BYTE_LENGTH / 2);

        if((hex_position + BYTE_LENGTH / 2) > hex_size) return;
        writeBuffer(hex, green_palette_slice, hex_position, hex_position + (BYTE_LENGTH / 2));
        hex_position += (BYTE_LENGTH / 2);
        
        if((hex_position + BYTE_LENGTH / 2) > hex_size) return;
        writeBuffer(hex, blue_palette_slice, hex_position, hex_position + (BYTE_LENGTH / 2));
        hex_position += (BYTE_LENGTH / 2);

        plte_size += (BYTE_LENGTH / 2) * 3;

        int red_palette = hexToInt(red_palette_slice, (BYTE_LENGTH / 2));
        int green_palette = hexToInt(green_palette_slice, (BYTE_LENGTH / 2));
        int blue_palette = hexToInt(blue_palette_slice, (BYTE_LENGTH / 2));

        png->plte.rgbs[rgb_size].r = red_palette;
        png->plte.rgbs[rgb_size].g = green_palette;
        png->plte.rgbs[rgb_size].b = blue_palette;
        png->plte.rgbs[rgb_size].a = 255;
        rgb_size++;

        if((hex_position + BYTE_LENGTH * 2) > hex_size) return;

        writeBuffer(hex, idat_header, hex_position + BYTE_LENGTH * 2, hex_position + BYTE_LENGTH * 3);
        idat_header[BYTE_LENGTH] = '\0';
    }

    hex_position += BYTE_LENGTH;

    *hp = hex_position;
}

void getAncillary(
    // PNG *png,
    char hex[],
    size_t hex_size,
    size_t *hp
) {
    size_t hex_position = *hp;

    char idat_header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        idat_header, 
        hex_position + BYTE_LENGTH,
        hex_position + BYTE_LENGTH * 2
    );
    
    idat_header[BYTE_LENGTH] = '\0';

    while (strcmp(idat_header, IDAT_SIG) != 0 && hex_position < hex_size) {
        if((hex_position + BYTE_LENGTH) > hex_size) return;

        char ancillary_header_size_slice[BYTE_LENGTH];
        writeBuffer(hex, ancillary_header_size_slice, hex_position, hex_position + BYTE_LENGTH);
        hex_position += BYTE_LENGTH;

        int ancillary_header_size = hexToInt(ancillary_header_size_slice, BYTE_LENGTH);
        if(ancillary_header_size < 0) break;

        if((hex_position + BYTE_LENGTH) > hex_size) return;

        char ancillary_header_slice[BYTE_LENGTH + 1];
        writeBuffer(hex, ancillary_header_slice, hex_position, hex_position + BYTE_LENGTH);
        hex_position += BYTE_LENGTH;
        ancillary_header_slice[BYTE_LENGTH] = '\0';

        // printf("ancillary header: %s\n", ancillary_header_slice);

        // const ancillary_header_data = hex[hex_position .. hex_position + 2 * ancillary_header_size];
        // _ = ancillary_header_data;
        hex_position += 2 * ancillary_header_size;

        // const ancillary_header_crc = hex[hex_position .. hex_position + BYTE_LENGTH];
        // _ = ancillary_header_crc;
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH * 2) > hex_size) return;

        writeBuffer(
            hex, 
            idat_header, 
            hex_position + BYTE_LENGTH,
            hex_position + BYTE_LENGTH * 2
        );
        
        idat_header[BYTE_LENGTH] = '\0';
    }

    *hp = hex_position;
}

void getIdat(
    PNG *png,
    char hex[],
    size_t hex_size,

    size_t *hp,
    int idat_index
) {
    size_t hex_position = *hp;

    png->idats[idat_index].size = 0;
    png->idats[idat_index].data = NULL;

    if((hex_position + BYTE_LENGTH) > hex_size) return;
    
    char idat_size_slice[BYTE_LENGTH];
    writeBuffer(hex, idat_size_slice, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;
    int idat_size = hexToInt(idat_size_slice, BYTE_LENGTH);

    if((hex_position + BYTE_LENGTH) > hex_size) return;

    char idat_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        idat_header_slice, 
        hex_position, 
        hex_position + BYTE_LENGTH
    );
    hex_position += BYTE_LENGTH;
    idat_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(idat_header_slice, IDAT_SIG) != 0) {
        hex_position += BYTE_LENGTH * 3; // skipping header, size, and crc
        hex_position += idat_size;

        *hp = hex_position;
        return;
    }
    png->idats[idat_index].size = (size_t)idat_size;

    if (idat_index > 0) {
        if((hex_position + idat_size * 2) > hex_size) return;
        
        png->idats[idat_index].data = malloc((idat_size * 2) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            hex_position,
            hex_position + (idat_size * 2)
        );

        hex_position += (idat_size * 2); // crc

        if((hex_position + BYTE_LENGTH * 4) > hex_size) return;

        char iend_header[BYTE_LENGTH + 1];
        writeBuffer(
            hex,
            iend_header,
            hex_position + BYTE_LENGTH * 3,
            hex_position + BYTE_LENGTH * 4
        );
        iend_header[BYTE_LENGTH] = '\0';

        int adler_zlib_checksum = 0;
        if (strcmp(iend_header, IEND_SIG) == 0) {
            if((hex_position + BYTE_LENGTH) > hex_size) return;

            char adler_zlib_checksum_slice[BYTE_LENGTH];
            writeBuffer(
                hex,
                adler_zlib_checksum_slice,
                hex_position,
                hex_position + BYTE_LENGTH
            );
            hex_position += BYTE_LENGTH;
            
            adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
        }

        if((hex_position + BYTE_LENGTH) > hex_size) return;
        char idat_png_crc[BYTE_LENGTH + 1];
        writeBuffer(
            hex,
            idat_png_crc,
            hex_position,
            hex_position + BYTE_LENGTH
        );
        hex_position += BYTE_LENGTH;
            
        // define IDAT
        png->idats[idat_index].compression_info = 0;
        png->idats[idat_index].compression_method = 0;
        png->idats[idat_index].zlib_fcheck_value = 0;
        png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
        // png->idats[idat_index].crc = idat_png_crc;

        *hp = hex_position;
        return;
    }

    if((hex_position + BIT_LENGTH) > hex_size) return;
    char deflate_compression_info[BIT_LENGTH];
    writeBuffer(hex, deflate_compression_info, hex_position, hex_position + BIT_LENGTH);
    hex_position += BIT_LENGTH;
    
    if((hex_position + BIT_LENGTH) > hex_size) return;
    char deflate_compression_method[BIT_LENGTH];
    writeBuffer(hex, deflate_compression_method, hex_position, hex_position + BIT_LENGTH);
    hex_position += BIT_LENGTH;
    
    if((hex_position + BIT_LENGTH * 2) > hex_size) return;
    char zlib_fcheck_value[BIT_LENGTH * 2];
    writeBuffer(hex, zlib_fcheck_value, hex_position, hex_position + BIT_LENGTH * 2);
    hex_position += BIT_LENGTH * 2;
    
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
        exit(0);
        return;
    }

    if((hex_position + BYTE_LENGTH * 3) > hex_size) return;
    char iend_header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_header, 
        hex_position + BYTE_LENGTH * 2, 
        hex_position + BYTE_LENGTH * 3
    );
    iend_header[BYTE_LENGTH] = '\0';

    int adler_zlib_checksum = 0;
    if (strcmp(iend_header, IEND_SIG) == 0) {
        if((hex_position + idat_size * 2 - 12) > hex_size) return;

        png->idats[idat_index].data = malloc((idat_size * 2 - 12) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            hex_position,
            hex_position + idat_size * 2 - 12
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2), ADLER32 (8)
        hex_position += (idat_size * 2) - 12;

        if((hex_position + BYTE_LENGTH) > hex_size) return;
        char adler_zlib_checksum_slice[BYTE_LENGTH];
        writeBuffer(
            hex, 
            adler_zlib_checksum_slice, 
            hex_position, 
            hex_position + BYTE_LENGTH
        );
        hex_position += BYTE_LENGTH;
        adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
    } else {
        if((hex_position + idat_size * 2 - 4) > hex_size) return;

        png->idats[idat_index].data = malloc((idat_size * 2 - 4) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            hex_position,
            hex_position + idat_size * 2 - 4
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2)
        hex_position += (idat_size * 2) - 4;
    }

    // char idat_png_crc[BYTE_LENGTH];
    // writeBuffer(hex, idat_png_crc, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    // define IDAT
    png->idats[idat_index].compression_info = hexToInt(deflate_compression_info, BYTE_LENGTH);
    png->idats[idat_index].compression_method = hexToInt(deflate_compression_method, BYTE_LENGTH);
    png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
    png->idats[idat_index].zlib_fcheck_value = validation_value;
    // png->idats[idat_index].crc = idat_png_crc;

    *hp = hex_position;
}

void getIend(
    PNG *png,
    char hex[],
    size_t hex_size,
    size_t *hp
) {
    size_t hex_position = *hp;

    if((hex_position + BYTE_LENGTH) > hex_size) return;

    char iend_size_slice[BYTE_LENGTH];
    writeBuffer(hex, iend_size_slice, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    png->iend.size = (size_t)hexToInt(iend_size_slice, BYTE_LENGTH);

    if((hex_position + BYTE_LENGTH) > hex_size) return;

    char iend_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_header_slice, 
        hex_position, 
        hex_position + BYTE_LENGTH
    );
    hex_position += BYTE_LENGTH;
    iend_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(iend_header_slice, IEND_SIG) != 0) {
        printf("not correct IEND header!");
        return;
    }

    hex_position += png->iend.size;

    if((hex_position + BYTE_LENGTH) > hex_size) return;

    char iend_png_src[BYTE_LENGTH];
    writeBuffer(hex, iend_png_src, hex_position, hex_position + BYTE_LENGTH);
    hex_position += BYTE_LENGTH;

    *hp = hex_position;
}

int countIdats(
    char hex[],
    size_t hex_size
) {
    int c = 0; // *counter;

    size_t hex_position = 0;

    // skip PNG header
    hex_position += BYTE_LENGTH * 2;

    if((hex_position + BYTE_LENGTH) > hex_size) {
        return c;
    }

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        size, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > hex_size) {
        return c;
    }

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
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

        if((hex_position + BYTE_LENGTH) > hex_size) {
            return c;
        }

        writeBuffer(
            hex, 
            size, 
            hex_position,
            hex_position + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH) > hex_size) {
            return c;
        }

        writeBuffer(
            hex, 
            header, 
            hex_position, 
            hex_position + BYTE_LENGTH
        );
        header[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;
    }

    return c;
}

int countRgbs(
    char hex[],
    size_t hex_size
) {
    int c = 0; // *counter;

    size_t hex_position = 0;

    // skip PNG header
    hex_position += BYTE_LENGTH * 2;
    if((hex_position + BYTE_LENGTH) > hex_size) {
        return c;
    }

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        size, 
        hex_position,
        hex_position + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_position += BYTE_LENGTH;

    if((hex_position + BYTE_LENGTH) > hex_size) {
        return c;
    }

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
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

        if((hex_position + BYTE_LENGTH) > hex_size) {
            return c;
        }

        writeBuffer(
            hex, 
            size, 
            hex_position,
            hex_position + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_position += BYTE_LENGTH;

        if((hex_position + BYTE_LENGTH) > hex_size) {
            return c;
        }

        writeBuffer(
            hex, 
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

void getPng(
    PNG *png,
    char hex[],
    size_t hex_size
)  {
    size_t hex_position = 0;

    if((hex_position + BYTE_LENGTH * 2) > hex_size) {
        return;
    }

    char png_signature[BYTE_LENGTH * 2 + 1];
    writeBuffer(hex, png_signature, hex_position, hex_position + BYTE_LENGTH * 2);
    hex_position += BYTE_LENGTH * 2;
    png_signature[BYTE_LENGTH * 2] = '\0';

    // verify PNG signature
    if(strcmp(png_signature, PNG_SIG) != 0) {
        printf("Invalid PNG signature");
        return;
    }

    getIhdr(png, hex, hex_size, &hex_position);
    getPlte(png, hex, hex_size, &hex_position);
    getAncillary(/* png, */ hex, hex_size, &hex_position);

    if((hex_position + BYTE_LENGTH * 2) > hex_size) {
        return;
    }

    char iend_signature[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_signature, 
        hex_position + BYTE_LENGTH, 
        hex_position + BYTE_LENGTH * 2
    );

    iend_signature[BYTE_LENGTH] = '\0';
    int idat_index = 0;

    while(strcmp(iend_signature, IEND_SIG) != 0) {
        getIdat(png, hex, hex_size, &hex_position, idat_index);
        idat_index++;

        if((hex_position + BYTE_LENGTH * 2) > hex_size) {
            return;
        }

        writeBuffer(
            hex,
            iend_signature,
            hex_position + BYTE_LENGTH,
            hex_position + BYTE_LENGTH * 2
        );
        iend_signature[BYTE_LENGTH] = '\0';
    }

    getIend(
        png,
        hex,
        hex_size,
        &hex_position
    );
}
