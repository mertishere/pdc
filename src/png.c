#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "utils.h"
#include "constants.h"


PNG initPng() {
    IDAT idats[10];
    RGB rgbs[10];

    PNG png = {
        .idats = idats,
        .iend = {
            .size = 0
        },
        .ihdr = {
            .bits_per_pixel = 0,
            .color_type = 0,
            .compression_method = 0,
            .filter_method = 0,
            .height = 0,
            .interlaced = 0,
            .size = 0,
            .width = 0,
        },
        .plte = {
            .rgbs = rgbs,
            .size = 0,
        }
    };
}


void getIhdr(PNG *png, char hex[], size_t *cbp) {
    size_t current_bit_position = *cbp;

    char ihdr_size_slice[BYTE_LENGTH];
    writeBuffer(hex, ihdr_size_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    int ihdr_size = hexToInt(ihdr_size_slice, BYTE_LENGTH);
    png->ihdr.size = ihdr_size;

    char ihdr_header_slice[BYTE_LENGTH + 1];
    writeBuffer(hex, ihdr_header_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    ihdr_header_slice[BYTE_LENGTH] = '\0';
    
    if (strcmp(ihdr_header_slice, IHDR_SIG) != 0) {
        printf("not correct IHDR header!");
        return;
    }

    char ihdr_png_width[BYTE_LENGTH];
    writeBuffer(hex, ihdr_png_width, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    char ihdr_png_height[BYTE_LENGTH];
    writeBuffer(hex, ihdr_png_height, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    char ihdr_png_bbp[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_bbp, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

    char ihdr_png_ct[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_ct, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

    char ihdr_png_cm[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_cm, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

    char ihdr_png_fm[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_fm, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

    char ihdr_png_ii[BIT_LENGTH * 2];
    writeBuffer(hex, ihdr_png_ii, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

    // char ihdr_png_crc[BYTE_LENGTH];
    // writeBuffer(hex, ihdr_png_crc, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    // define IHDR
    png->ihdr.width = hexToInt(ihdr_png_width, BYTE_LENGTH);
    png->ihdr.height = hexToInt(ihdr_png_height, BYTE_LENGTH);
    png->ihdr.bits_per_pixel = hexToInt(ihdr_png_bbp, BIT_LENGTH * 2);
    png->ihdr.color_type = hexToInt(ihdr_png_ct, BIT_LENGTH * 2);
    png->ihdr.compression_method = hexToInt(ihdr_png_cm, BIT_LENGTH * 2);
    png->ihdr.filter_method = hexToInt(ihdr_png_fm, BIT_LENGTH * 2);
    png->ihdr.interlaced = hexToInt(ihdr_png_ii, BIT_LENGTH * 2) == 1;
    // png->ihdr.crc = ihdr_png_crc;

    *cbp = current_bit_position;
}

void getPlte(
    PNG *png,
    char hex[],
    size_t *cbp
) {
    size_t current_bit_position = *cbp;

    if (png->ihdr.color_type == 0 || png->ihdr.color_type == 4) return;

    char plte_size_slice[BYTE_LENGTH];
    writeBuffer(hex, plte_size_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    char plte_header[BYTE_LENGTH + 1];
    writeBuffer(hex, plte_header, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;
    plte_header[BYTE_LENGTH] = '\0';

    if(strcmp(plte_header, PLTE_SIG) != 0) {
        current_bit_position -= BYTE_LENGTH * 2;
        return;
    }

    png->plte.size = hexToInt(plte_size_slice, BYTE_LENGTH);

    char idat_header[BYTE_LENGTH + 1];
    writeBuffer(hex, idat_header, current_bit_position + BYTE_LENGTH * 2, current_bit_position + BYTE_LENGTH * 3);
    idat_header[BYTE_LENGTH] = '\0';

    size_t hex_size = strlen(hex);
    size_t rgb_size = 0;

    size_t plte_size = 0;
    while (plte_size < (png->plte.size * 2)) {
        if(hex_size < current_bit_position) break;
        char red_palette_slice[(BYTE_LENGTH / 2)];
        char green_palette_slice[(BYTE_LENGTH / 2)];
        char blue_palette_slice[(BYTE_LENGTH / 2)];
        
        writeBuffer(hex, red_palette_slice, current_bit_position, current_bit_position + (BYTE_LENGTH / 2));
        current_bit_position += (BYTE_LENGTH / 2);
        writeBuffer(hex, green_palette_slice, current_bit_position, current_bit_position + (BYTE_LENGTH / 2));
        current_bit_position += (BYTE_LENGTH / 2);
        writeBuffer(hex, blue_palette_slice, current_bit_position, current_bit_position + (BYTE_LENGTH / 2));
        current_bit_position += (BYTE_LENGTH / 2);

        plte_size += (BYTE_LENGTH / 2) * 3;

        int red_palette = hexToInt(red_palette_slice, (BYTE_LENGTH / 2));
        int green_palette = hexToInt(green_palette_slice, (BYTE_LENGTH / 2));
        int blue_palette = hexToInt(blue_palette_slice, (BYTE_LENGTH / 2));

        png->plte.rgbs[rgb_size].r = red_palette;
        png->plte.rgbs[rgb_size].g = green_palette;
        png->plte.rgbs[rgb_size].b = blue_palette;
        png->plte.rgbs[rgb_size].a = 255;
        rgb_size++;

        writeBuffer(hex, idat_header, current_bit_position + BYTE_LENGTH * 2, current_bit_position + BYTE_LENGTH * 3);
        idat_header[BYTE_LENGTH] = '\0';
    }

    current_bit_position += BYTE_LENGTH;

    *cbp = current_bit_position;
}

void getAncillary(PNG *png, char hex[], size_t *cbp) {
    size_t current_bit_position = *cbp;

    char idat_header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        idat_header, 
        current_bit_position + BYTE_LENGTH,
        current_bit_position + BYTE_LENGTH * 2
    );
    
    idat_header[BYTE_LENGTH] = '\0';

    size_t hex_size = strlen(hex);
    // INIT ANCILLARY CHUNKS
    while (strcmp(idat_header, IDAT_SIG) != 0 && current_bit_position < hex_size) {
        char ancillary_header_size_slice[BYTE_LENGTH];
        writeBuffer(hex, ancillary_header_size_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
        current_bit_position += BYTE_LENGTH;

        int ancillary_header_size = hexToInt(ancillary_header_size_slice, BYTE_LENGTH);

        char ancillary_header_slice[BYTE_LENGTH + 1];
        writeBuffer(hex, ancillary_header_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
        current_bit_position += BYTE_LENGTH;
        ancillary_header_slice[BYTE_LENGTH] = '\0';

        // printf("ancillary header: %s\n", ancillary_header_slice);

        // const ancillary_header_data = hex[current_bit_position .. current_bit_position + 2 * ancillary_header_size];
        // _ = ancillary_header_data;
        current_bit_position += 2 * ancillary_header_size;

        // const ancillary_header_crc = hex[current_bit_position .. current_bit_position + BYTE_LENGTH];
        // _ = ancillary_header_crc;
        current_bit_position += BYTE_LENGTH;

        writeBuffer(
            hex, 
            idat_header, 
            current_bit_position + BYTE_LENGTH,
            current_bit_position + BYTE_LENGTH * 2
        );
        
        idat_header[BYTE_LENGTH] = '\0';
    }

    *cbp = current_bit_position;
}

void getIdat(PNG *png, char hex[], size_t *cbp, int idat_index) {
    int current_bit_position = *cbp;

    // INIT IDAT
    // get PNG IDAT size
    char idat_size_slice[BYTE_LENGTH];
    writeBuffer(hex, idat_size_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;
    
    int idat_size = hexToInt(idat_size_slice, BYTE_LENGTH);

    char idat_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        idat_header_slice, 
        current_bit_position, 
        current_bit_position + BYTE_LENGTH
    );
    current_bit_position += BYTE_LENGTH;
    idat_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(idat_header_slice, IDAT_SIG) != 0) {
        current_bit_position += BYTE_LENGTH * 3; // skipping header, size, and crc
        current_bit_position += idat_size;

        *cbp = current_bit_position;
        // ("not correct IDAT header! (%s)\n", idat_header_slice);
        return;
    }
    png->idats[idat_index].size = idat_size;

    if (idat_index > 0) {
        png->idats[idat_index].data = malloc((idat_size * 2) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            current_bit_position,
            current_bit_position + (idat_size * 2)
        );

        current_bit_position += (idat_size * 2); // =>  CRC

        char iend_header[BYTE_LENGTH + 1];
        writeBuffer(
            hex,
            iend_header,
            current_bit_position + BYTE_LENGTH * 3,
            current_bit_position + BYTE_LENGTH * 4
        );
        iend_header[BYTE_LENGTH] = '\0';

        int adler_zlib_checksum = 0;
        if (strcmp(iend_header, IEND_SIG) == 0) {
            char adler_zlib_checksum_slice[BYTE_LENGTH];
            writeBuffer(
                hex,
                adler_zlib_checksum_slice,
                current_bit_position,
                current_bit_position + BYTE_LENGTH
            );
            current_bit_position += BYTE_LENGTH;
            
            adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
        }

        char idat_png_crc[BYTE_LENGTH + 1];
        writeBuffer(
            hex,
            idat_png_crc,
            current_bit_position,
            current_bit_position + BYTE_LENGTH
        );
        current_bit_position += BYTE_LENGTH;
            
        // define IDAT
        png->idats[idat_index].compression_info = 0;
        png->idats[idat_index].compression_method = 0;
        png->idats[idat_index].zlib_fcheck_value = 0;
        png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
        // png->idats[idat_index].crc = idat_png_crc;

        *cbp = current_bit_position;
        return;
    }

    char deflate_compression_info[BIT_LENGTH];
    writeBuffer(hex, deflate_compression_info, current_bit_position, current_bit_position + BIT_LENGTH);
    current_bit_position += BIT_LENGTH;

    char deflate_compression_method[BIT_LENGTH];
    writeBuffer(hex, deflate_compression_method, current_bit_position, current_bit_position + BIT_LENGTH);
    current_bit_position += BIT_LENGTH;

    char zlib_fcheck_value[BIT_LENGTH * 2];
    writeBuffer(hex, zlib_fcheck_value, current_bit_position, current_bit_position + BIT_LENGTH * 2);
    current_bit_position += BIT_LENGTH * 2;

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


    char iend_header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_header, 
        current_bit_position + BYTE_LENGTH * 2, 
        current_bit_position + BYTE_LENGTH * 3
    );
    iend_header[BYTE_LENGTH] = '\0';

    int adler_zlib_checksum = 0;
    if (strcmp(iend_header, IEND_SIG) == 0) {
        png->idats[idat_index].data = malloc((idat_size * 2 - 12) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            current_bit_position,
            current_bit_position + idat_size * 2 - 12
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2), ADLER32 (8)
        current_bit_position += (idat_size * 2) - 12;

        char adler_zlib_checksum_slice[BYTE_LENGTH];
        writeBuffer(
            hex, 
            adler_zlib_checksum_slice, 
            current_bit_position, 
            current_bit_position + BYTE_LENGTH
        );
        current_bit_position += BYTE_LENGTH;
        adler_zlib_checksum = hexToInt(adler_zlib_checksum_slice, BYTE_LENGTH);
    } else {
        png->idats[idat_index].data = malloc((idat_size * 2 - 4) * sizeof(char));
        writeBuffer(
            hex,
            png->idats[idat_index].data,
            current_bit_position,
            current_bit_position + idat_size * 2 - 4
        );

        // => ZLIB CHECK VALUE (2), COMPRESSION METHOD (2)
        current_bit_position += (idat_size * 2) - 4;
    }

    // char idat_png_crc[BYTE_LENGTH];
    // writeBuffer(hex, idat_png_crc, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    // define IDAT
    png->idats[idat_index].compression_info = hexToInt(deflate_compression_info, BYTE_LENGTH);
    png->idats[idat_index].compression_method = hexToInt(deflate_compression_method, BYTE_LENGTH);
    png->idats[idat_index].adler_zlib_checksum = adler_zlib_checksum;
    png->idats[idat_index].zlib_fcheck_value = validation_value;
    // png->idats[idat_index].crc = idat_png_crc;

    *cbp = current_bit_position;
}

void getIend(PNG *png, char hex[], size_t *cbp, int idat_index) {
    size_t current_bit_position = *cbp;

    // INIT IDAT
    // get PNG IDAT size
    char iend_size_slice[BYTE_LENGTH];
    writeBuffer(hex, iend_size_slice, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    png->iend.size = hexToInt(iend_size_slice, BYTE_LENGTH);

    char iend_header_slice[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_header_slice, 
        current_bit_position, 
        current_bit_position + BYTE_LENGTH
    );
    current_bit_position += BYTE_LENGTH;
    iend_header_slice[BYTE_LENGTH] = '\0';

    if (strcmp(iend_header_slice, IEND_SIG) != 0) {
        printf("not correct IEND header!");
        return;
    }

    current_bit_position += png->iend.size;

    char iend_png_src[BYTE_LENGTH];
    writeBuffer(hex, iend_png_src, current_bit_position, current_bit_position + BYTE_LENGTH);
    current_bit_position += BYTE_LENGTH;

    *cbp = current_bit_position;
}

int countIdats(char hex[]) {
    int c = 0; // *counter;

    size_t hex_len = 0;

    // skip PNG header
    hex_len += BYTE_LENGTH * 2;

    char size[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        size, 
        hex_len,
        hex_len + BYTE_LENGTH
    );
    size[BYTE_LENGTH] = '\0';
    hex_len += BYTE_LENGTH;

    char header[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        header, 
        hex_len,
        hex_len + BYTE_LENGTH
    );
    header[BYTE_LENGTH] = '\0';
    hex_len += BYTE_LENGTH;

    while(strcmp(header, IEND_SIG) != 0) {
        if(strcmp(header, IDAT_SIG) == 0) c++;
        hex_len += hexToInt(size, BYTE_LENGTH) * 2;
        hex_len += BYTE_LENGTH; // crc

        writeBuffer(
            hex, 
            size, 
            hex_len,
            hex_len + BYTE_LENGTH
        );
        size[BYTE_LENGTH] = '\0';
        hex_len += BYTE_LENGTH;

        writeBuffer(
            hex, 
            header, 
            hex_len, 
            hex_len + BYTE_LENGTH
        );
        header[BYTE_LENGTH] = '\0';
        hex_len += BYTE_LENGTH;
    }

    return c;
}

void getPng(PNG *png, char hex[])  {
    size_t current_bit_position = 0;

    char png_signature[BYTE_LENGTH * 2 + 1];
    writeBuffer(hex, png_signature, current_bit_position, current_bit_position + BYTE_LENGTH * 2);
    current_bit_position += BYTE_LENGTH * 2;
    png_signature[BYTE_LENGTH * 2] = '\0';

    // verify PNG signature
    if(strcmp(png_signature, PNG_SIG) != 0) {
        printf("Invalid PNG signature");
        return;
    }

    getIhdr(png, hex, &current_bit_position);
    getPlte(png, hex, &current_bit_position);
    getAncillary(png, hex, &current_bit_position);

    char iend_signature[BYTE_LENGTH + 1];
    writeBuffer(
        hex, 
        iend_signature, 
        current_bit_position + BYTE_LENGTH, 
        current_bit_position + BYTE_LENGTH * 2
    );

    iend_signature[BYTE_LENGTH] = '\0';
    int idat_index = 0;

    while(strcmp(iend_signature, IEND_SIG) != 0) {
        getIdat(png, hex, &current_bit_position, idat_index);
        idat_index++;

        writeBuffer(
            hex,
            iend_signature,
            current_bit_position + BYTE_LENGTH,
            current_bit_position + BYTE_LENGTH * 2
        );
        iend_signature[BYTE_LENGTH] = '\0';
    }
    getIend(png, hex, &current_bit_position, 0);
}
