#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "utils.h"
#include "constants.h"

const char *gAMA_signature = "67414d41";
const char *cHRM_signature = "6348524d";
const char *sRGB_signature = "73524742";
const char *bKGD_signature = "624b4744";
const char *pHYs_signature = "70485973";
const char *sBIT_signature = "73424954";
const char *sPLT_signature = "73504c54";
const char *tIME_signature = "74494d45";
const char *tEXt_signature = "74455874";

void gAMA(ANCI ancillary) {
    char *d = ancillary.data;
    int big_gamma = hexToInt(d, 8);
    float gamma = ((float)big_gamma) / 100000;
    printf("gamma %.5f\n", gamma);
}

void cHRM(ANCI ancillary) {
    char *d = ancillary.data;

    char white_point_x_slice[BYTE_LENGTH];
    char white_point_y_slice[BYTE_LENGTH];
    char red_x_slice[BYTE_LENGTH];
    char red_y_slice[BYTE_LENGTH];
    char green_x_slice[BYTE_LENGTH];
    char green_y_slice[BYTE_LENGTH];
    char blue_x_slice[BYTE_LENGTH];
    char blue_y_slice[BYTE_LENGTH];

    size_t start = 0;

    writeBuffer(d, white_point_x_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, white_point_y_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, red_x_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, red_y_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, green_x_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, green_y_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, blue_x_slice, start, start + BYTE_LENGTH);
    start += BYTE_LENGTH;
    writeBuffer(d, blue_y_slice, start, start + BYTE_LENGTH);

    int white_point_x = hexToInt(white_point_x_slice, BYTE_LENGTH);
    int white_point_y = hexToInt(white_point_y_slice, BYTE_LENGTH);
    int red_x = hexToInt(red_x_slice, BYTE_LENGTH);
    int red_y = hexToInt(red_y_slice, BYTE_LENGTH);
    int green_x = hexToInt(green_x_slice, BYTE_LENGTH);
    int green_y = hexToInt(green_y_slice, BYTE_LENGTH);
    int blue_x = hexToInt(blue_x_slice, BYTE_LENGTH);
    int blue_y = hexToInt(blue_y_slice, BYTE_LENGTH);

    printf("white_point_x %d\n", white_point_x);
    printf("white_point_y %d\n", white_point_y);
    printf("red_x %d\n", red_x);
    printf("red_y %d\n", red_y);
    printf("green_x %d\n", green_x);
    printf("green_y %d\n", green_y);
    printf("blue_x %d\n", blue_x);
    printf("blue_y %d\n", blue_y);
}

void sRGB(ANCI ancillary) {
    char *d = ancillary.data;

    char rendering_intent_slice[2];

    writeBuffer(d, rendering_intent_slice, 0, 2);
    int rendering_intent = hexToInt(rendering_intent_slice, 2);
    
    printf("rendering_intent %d\n", rendering_intent);
}

void bKGD(ANCI ancillary, int ct) {
    char *d = ancillary.data;

    if(ct == 3) {
        char palette_index_slice[2];
    
        writeBuffer(d, palette_index_slice, 0, 2);
        int palette_index = hexToInt(palette_index_slice, 2);
    
        printf("palette_index %d\n", palette_index);
    } else if(ct == 0 || ct == 4) {
        char gray_slice[4];
    
        writeBuffer(d, gray_slice, 0, 4);
        int gray = hexToInt(gray_slice, 4);
    
        printf("gray %d\n", gray);
    } else if(ct == 2 || ct == 6) {
        size_t start = 0;

        char red_slice[4];
        writeBuffer(d, red_slice, start, start + 4);
        int red = hexToInt(red_slice, 4);
        printf("red %d\n", red);
        start += 4;

        char green_slice[4];
        writeBuffer(d, green_slice, start, start + 4);
        int green = hexToInt(green_slice, 4);
        printf("green %d\n", green);
        start += 4;

        char blue_slice[4];
        writeBuffer(d, blue_slice, start, start + 4);
        int blue = hexToInt(blue_slice, 4);
        printf("blue %d\n", blue);
        start += 4;
    }
}

void pHYs(ANCI ancillary) {
    char *d = ancillary.data;

    char pixels_per_unit_x_slice[8];
    char pixels_per_unit_y_slice[8];
    char unit_specifier_slice[2];

    size_t start = 0;

    writeBuffer(d, pixels_per_unit_x_slice, start, start + 8);
    start += 8;

    writeBuffer(d, pixels_per_unit_y_slice, start, start + 8);
    start += 8;

    writeBuffer(d, unit_specifier_slice, start, start + 2);

    int pixels_per_unit_x = hexToInt(pixels_per_unit_x_slice, 8);
    int pixels_per_unit_y = hexToInt(pixels_per_unit_y_slice, 8);
    int unit_specifier = hexToInt(unit_specifier_slice, 2);

    printf("pixels_per_unit_x %d\n", pixels_per_unit_x);
    printf("pixels_per_unit_y %d\n", pixels_per_unit_y);
    printf("unit_specifier %d\n", unit_specifier);

}

void sBIT(ANCI ancillary) {
    if(ancillary.data != NULL) return;
    // TODO
    // char *d = ancillary.data;
}

void sPLT(ANCI ancillary) {
    // TODO
    char *d = ancillary.data;

    size_t start = 0;
    while((start + 1) < (ancillary.size * 2)) {
        if(ancillary.data[start] == '0' && ancillary.data[start + 1] == '0') {
            start += 2;
            break;
        }
        start += 2;
        continue;
    }

    char sample_depth_size_slice[2]; // 1 byte
    writeBuffer(
        d,
        sample_depth_size_slice,
        start,
        start + 2
    );

    start += 2;
    int sample_depth_size = hexToInt(sample_depth_size_slice, 2);
    if(sample_depth_size != 8 && sample_depth_size != 16)  {
        printf("Error, invalid depth size %d\n", sample_depth_size);
        exit(EXIT_FAILURE);
    }

    int rgb_size = 1;
    if(sample_depth_size == 16) rgb_size = 2;

    char r_slice[rgb_size];
    writeBuffer(d, r_slice, start, start + rgb_size);
    start += rgb_size;
    int r = hexToInt(r_slice, rgb_size);

    char g_slice[rgb_size];
    writeBuffer(d, g_slice, start, start + rgb_size);
    start += rgb_size;
    int g = hexToInt(g_slice, rgb_size);

    char b_slice[rgb_size];
    writeBuffer(d, b_slice, start, start + rgb_size);
    start += rgb_size;
    int b = hexToInt(b_slice, rgb_size);

    char a_slice[rgb_size];
    writeBuffer(d, a_slice, start, start + rgb_size);
    start += rgb_size;
    int a = hexToInt(a_slice, rgb_size);

    char frequency_slice[2];
    writeBuffer(d, frequency_slice, start, start + 2);
    start += 2;
    int frequency = hexToInt(frequency_slice, 2);


    printf("sample_depth_size %d\n", sample_depth_size);
    printf("r %d\n", r);
    printf("g %d\n", g);
    printf("b %d\n", b);
    printf("a %d\n", a);
    printf("frequency %d\n", frequency);
}

void tIME(ANCI ancillary) {
    char *d = ancillary.data;

    size_t start = 0;
    char year_slice[4];
    writeBuffer(d, year_slice, start, start + 4);
    start += 4;
    int year = hexToInt(year_slice, 4);

    char month_slice[2];
    writeBuffer(d, month_slice, start, start + 2);
    start += 2;
    int month = hexToInt(month_slice, 2);

    char day_slice[2];
    writeBuffer(d, day_slice, start, start + 2);
    start += 2;
    int day = hexToInt(day_slice, 2);

    char hour_slice[2];
    writeBuffer(d, hour_slice, start, start + 2);
    start += 2;
    int hour = hexToInt(hour_slice, 2);

    char minute_slice[2];
    writeBuffer(d, minute_slice, start, start + 2);
    start += 2;
    int minute = hexToInt(minute_slice, 2);

    char second_slice[2];
    writeBuffer(d, second_slice, start, start + 2);
    start += 2;
    int second = hexToInt(second_slice, 2);


    printf("year %d\n", year);
    printf("month %d\n", month);
    printf("day %d\n", day);
    printf("hour %d\n", hour);
    printf("minute %d\n", minute);
    printf("second %d\n", second);
}

void tEXt(ANCI ancillary) {
    char *d = ancillary.data;

    size_t start = 0;
    while((start + 1) < (ancillary.size * 2)) {
        if(ancillary.data[start] == '0' && ancillary.data[start + 1] == '0') break;
        start += 2;
        continue;
    }
    char keyword[(start) + BIT_LENGTH];
    writeBuffer(
        d,
        keyword,
        0,
        start
    );
    keyword[start] = '\0';
    start += 2; // skip NULL byte

    size_t data_length = (ancillary.size * 2) - start;
    char data[data_length + BIT_LENGTH];

    writeBuffer(
        d,
        data,
        start,
        start + data_length
    );
    data[data_length] = '\0';

    printf("%s => %s\n", keyword, data);
}

void handleAncillaries(PNG png, size_t ancillaries_amount) {
    for(size_t i = 0; i < ancillaries_amount; i++) {
        ANCI a = png.ancillaries[i];

        if(strcmp(a.signature, gAMA_signature) == 0) gAMA(a);
        else if(strcmp(a.signature, cHRM_signature) == 0) cHRM(a);
        else if(strcmp(a.signature, sRGB_signature) == 0) sRGB(a);
        else if(strcmp(a.signature, bKGD_signature) == 0) bKGD(a, png.ihdr.color_type);
        else if(strcmp(a.signature, pHYs_signature) == 0) pHYs(a);
        else if(strcmp(a.signature, sBIT_signature) == 0) sBIT(a);
        else if(strcmp(a.signature, sPLT_signature) == 0) sPLT(a);
        else if(strcmp(a.signature, tIME_signature) == 0) tIME(a);
        else if(strcmp(a.signature, tEXt_signature) == 0) tEXt(a);
        else {
            printf("Unknown chunk header: %s\n", a.signature);
            continue;
        }
    }
}