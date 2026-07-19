#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "png.h"
#include "utils.h"


void getFilterRgbA(
    int uncompressed[], 
    size_t uncompressed_len,
    
    Pixel pixels[],
    size_t *pl,

    int image_height,
    int row_size
) {    
    size_t pixels_position = *pl;

    int current_filter_method = 0;
    int height_index = 0;
    for (int i = 0; i < image_height; i++) {
        int row_index = 0;
        while (true) {
            int height_row_size = height_index * row_size;
            int bit_index = height_row_size + row_index;
            
            if (bit_index >= row_size * (height_index + 1)) break;
            if ((size_t)bit_index >= uncompressed_len) break;
            if (row_index == 0) {
                current_filter_method = uncompressed[bit_index];
                row_index += 1;
                continue;
            }

            if (current_filter_method == 0) { // NO FILTER
                int r_bit = uncompressed[bit_index];
                int g_bit = uncompressed[bit_index + 1];
                int b_bit = uncompressed[bit_index + 2];
                int a_bit = uncompressed[bit_index + 3];

                pixels[pixels_position].rgb.r = r_bit;
                pixels[pixels_position].rgb.g = g_bit;
                pixels[pixels_position].rgb.b = b_bit;
                pixels[pixels_position].rgb.a = a_bit;

                pixels[pixels_position].col = height_index;
                pixels[pixels_position].row = (row_index - 1) / 4;
                pixels_position++;

                row_index += 4;
                continue;
            }

            int final_r_bit = uncompressed[bit_index];
            int final_g_bit = uncompressed[bit_index + 1];
            int final_b_bit = uncompressed[bit_index + 2];
            int final_a_bit = uncompressed[bit_index + 3];

            if (current_filter_method == 1) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;
                int previous_a_bit = 0;

                if (row_index >= 4) {
                    previous_r_bit = uncompressed[bit_index - 4];
                    previous_g_bit = uncompressed[bit_index - 3];
                    previous_b_bit = uncompressed[bit_index - 2];
                    previous_a_bit = uncompressed[bit_index - 1];
                }

                final_r_bit = (final_r_bit + previous_r_bit) % 256;
                final_g_bit = (final_g_bit + previous_g_bit) % 256;
                final_b_bit = (final_b_bit + previous_b_bit) % 256;
                final_a_bit = (final_a_bit + previous_a_bit) % 256;
            } else if (current_filter_method == 2) {
                int row_move_up = row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;
                int top_a_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                    top_a_bit = uncompressed[bit_index + 3 - row_move_up];
                }

                final_r_bit = (final_r_bit + top_r_bit) % 256;
                final_g_bit = (final_g_bit + top_g_bit) % 256;
                final_b_bit = (final_b_bit + top_b_bit) % 256;
                final_a_bit = (final_a_bit + top_a_bit) % 256;
            } else if (current_filter_method == 3) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;
                int previous_a_bit = 0;

                if (row_index >= 4) {
                    previous_r_bit = uncompressed[bit_index - 4];
                    previous_g_bit = uncompressed[bit_index - 3];
                    previous_b_bit = uncompressed[bit_index - 2];
                    previous_a_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;
                int top_a_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                    top_a_bit = uncompressed[bit_index + 3 - row_move_up];
                }

                final_r_bit = (final_r_bit + ((top_r_bit + previous_r_bit) / 2)) % 256;
                final_g_bit = (final_g_bit + ((top_g_bit + previous_g_bit) / 2)) % 256;
                final_b_bit = (final_b_bit + ((top_b_bit + previous_b_bit) / 2)) % 256;
                final_a_bit = (final_a_bit + ((top_a_bit + previous_a_bit) / 2)) % 256;
            } else if (current_filter_method == 4) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;
                int previous_a_bit = 0;

                if (row_index >= 4) {
                    previous_r_bit = uncompressed[bit_index - 4];
                    previous_g_bit = uncompressed[bit_index - 3];
                    previous_b_bit = uncompressed[bit_index - 2];
                    previous_a_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;
                int top_a_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                    top_a_bit = uncompressed[bit_index + 3 - row_move_up];
                }

                int top_left_r_bit = 0;
                int top_left_g_bit = 0;
                int top_left_b_bit = 0;
                int top_left_a_bit = 0;

                if (height_index > 0 && row_index >= 4) {
                    top_left_r_bit = uncompressed[bit_index - row_move_up - 4];
                    top_left_g_bit = uncompressed[bit_index - row_move_up - 3];
                    top_left_b_bit = uncompressed[bit_index - row_move_up - 2];
                    top_left_a_bit = uncompressed[bit_index - row_move_up - 1];
                }

                int r_predictor;
                int r_bit_p = previous_r_bit + top_r_bit - top_left_r_bit;
                int r_bit_a = abs(r_bit_p - previous_r_bit);
                int r_bit_b = abs(r_bit_p - top_r_bit);
                int r_bit_c = abs(r_bit_p - top_left_r_bit);

                if (r_bit_a <= r_bit_b && r_bit_a <= r_bit_c) {
                    r_predictor = previous_r_bit;
                } else if (r_bit_b <= r_bit_c) {
                    r_predictor = top_r_bit;
                } else {
                    r_predictor = top_left_r_bit;
                }

                final_r_bit = (final_r_bit + r_predictor) % 256;

                int g_predictor;
                int g_bit_p = previous_g_bit + top_g_bit - top_left_g_bit;
                int g_bit_a = abs(g_bit_p - previous_g_bit);
                int g_bit_b = abs(g_bit_p - top_g_bit);
                int g_bit_c = abs(g_bit_p - top_left_g_bit);

                if (g_bit_a <= g_bit_b && g_bit_a <= g_bit_c) {
                    g_predictor = previous_g_bit;
                } else if (g_bit_b <= g_bit_c) {
                    g_predictor = top_g_bit;
                } else {
                    g_predictor = top_left_g_bit;
                }

                final_g_bit = (final_g_bit + g_predictor) % 256;

                int b_predictor;
                int b_bit_p = previous_b_bit + top_b_bit - top_left_b_bit;
                int b_bit_a = abs(b_bit_p - previous_b_bit);
                int b_bit_b = abs(b_bit_p - top_b_bit);
                int b_bit_c = abs(b_bit_p - top_left_b_bit);

                if (b_bit_a <= b_bit_b && b_bit_a <= b_bit_c) {
                    b_predictor = previous_b_bit;
                } else if (b_bit_b <= b_bit_c) {
                    b_predictor = top_b_bit;
                } else {
                    b_predictor = top_left_b_bit;
                }

                final_b_bit = (final_b_bit + b_predictor) % 256;

                int a_predictor;
                int a_bit_p = previous_a_bit + top_a_bit - top_left_a_bit;
                int a_bit_a = abs(a_bit_p - previous_a_bit);
                int a_bit_b = abs(a_bit_p - top_a_bit);
                int a_bit_c = abs(a_bit_p - top_left_a_bit);

                if (a_bit_a <= a_bit_b && a_bit_a <= a_bit_c) {
                    a_predictor = previous_a_bit;
                } else if (a_bit_b <= a_bit_c) {
                    a_predictor = top_a_bit;
                } else {
                    a_predictor = top_left_a_bit;
                }

                final_a_bit = (final_a_bit + a_predictor) % 256;
            } else {
                printf("Invalid filter method! %d\n", current_filter_method);
                exit(0);
            }

            int r_bit = final_r_bit;
            int g_bit = final_g_bit;
            int b_bit = final_b_bit;
            int a_bit = final_a_bit;

            uncompressed[bit_index] = r_bit;
            uncompressed[bit_index + 1] = g_bit;
            uncompressed[bit_index + 2] = b_bit;
            uncompressed[bit_index + 3] = a_bit;

            pixels[pixels_position].rgb.r = r_bit;
            pixels[pixels_position].rgb.g = g_bit;
            pixels[pixels_position].rgb.b = b_bit;
            pixels[pixels_position].rgb.a = a_bit;

            pixels[pixels_position].col = height_index;
            pixels[pixels_position].row = (row_index - 1) / 4;
            pixels_position++;

            row_index += 4;
        }
        height_index += 1;
    }

    *pl = pixels_position;
}

void getFilterRgb(
    int uncompressed[], 
    size_t uncompressed_len,
    
    Pixel pixels[],
    size_t *pl,

    int image_height,
    int row_size
) {    
    size_t pixels_position = *pl;

    int current_filter_method = 0;
    int height_index = 0;
    for (int i = 0; i < image_height; i++) {
        int row_index = 0;
        while (true) {
            int height_row_size = height_index * row_size;
            int bit_index = height_row_size + row_index;

            if (bit_index >= row_size * (height_index + 1)) break;
            if ((size_t)bit_index >= uncompressed_len) break;
            if (row_index == 0) {
                current_filter_method = uncompressed[bit_index];
                row_index += 1;
                continue;
            }

            if (current_filter_method == 0) { // NO FILTER
                int r_bit = uncompressed[bit_index];
                int g_bit = uncompressed[bit_index + 1];
                int b_bit = uncompressed[bit_index + 2];

                pixels[pixels_position].rgb.r = r_bit;
                pixels[pixels_position].rgb.g = g_bit;
                pixels[pixels_position].rgb.b = b_bit;
                pixels[pixels_position].rgb.a = 255;

                pixels[pixels_position].col = height_index;
                pixels[pixels_position].row = (row_index - 1) / 4;
                pixels_position++;

                row_index += 3;
                continue;
            }

            int final_r_bit = uncompressed[bit_index];
            int final_g_bit = uncompressed[bit_index + 1];
            int final_b_bit = uncompressed[bit_index + 2];

            if (current_filter_method == 1) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;

                if (row_index >= 3) {
                    previous_r_bit = uncompressed[bit_index - 3];
                    previous_g_bit = uncompressed[bit_index - 2];
                    previous_b_bit = uncompressed[bit_index - 1];
                }

                final_r_bit = (final_r_bit + previous_r_bit) % 256;
                final_g_bit = (final_g_bit + previous_g_bit) % 256;
                final_b_bit = (final_b_bit + previous_b_bit) % 256;
            } else if (current_filter_method == 2) {
                int row_move_up = height_row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                }

                final_r_bit = (final_r_bit + top_r_bit) % 256;
                final_g_bit = (final_g_bit + top_g_bit) % 256;
                final_b_bit = (final_b_bit + top_b_bit) % 256;
            } else if (current_filter_method == 3) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;

                if (row_index >= 3) {
                    previous_r_bit = uncompressed[bit_index - 3];
                    previous_g_bit = uncompressed[bit_index - 2];
                    previous_b_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                }

                final_r_bit = (final_r_bit + ((top_r_bit + previous_r_bit) / 2) % 256);
                final_g_bit = (final_g_bit + ((top_g_bit + previous_g_bit) / 2) % 256);
                final_b_bit = (final_b_bit + ((top_b_bit + previous_b_bit) / 2) % 256);
            } else if (current_filter_method == 4) {
                int previous_r_bit = 0;
                int previous_g_bit = 0;
                int previous_b_bit = 0;

                if (row_index >= 3) {
                    previous_r_bit = uncompressed[bit_index - 3];
                    previous_g_bit = uncompressed[bit_index - 2];
                    previous_b_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_r_bit = 0;
                int top_g_bit = 0;
                int top_b_bit = 0;

                if (height_index > 0) {
                    top_r_bit = uncompressed[bit_index - row_move_up];
                    top_g_bit = uncompressed[bit_index + 1 - row_move_up];
                    top_b_bit = uncompressed[bit_index + 2 - row_move_up];
                }

                int top_left_r_bit = 0;
                int top_left_g_bit = 0;
                int top_left_b_bit = 0;

                if (height_index > 0 && row_index >= 4) {
                    top_left_r_bit = uncompressed[bit_index - row_move_up - 1];
                    top_left_g_bit = uncompressed[bit_index + 1 - row_move_up - 1];
                    top_left_b_bit = uncompressed[bit_index + 2 - row_move_up - 1];
                }

                int r_bit_P = previous_r_bit + top_r_bit - top_left_r_bit;
                int r_bit_A = abs(r_bit_P - previous_r_bit);
                int r_bit_B = abs(r_bit_P - top_r_bit);
                int r_bit_C = abs(r_bit_P - top_left_r_bit);
                if (r_bit_A <= r_bit_B && r_bit_A <= r_bit_C) {
                    final_r_bit = (final_r_bit + r_bit_A) % 256;
                } else if (r_bit_B <= r_bit_C) {
                    final_r_bit = (final_r_bit + r_bit_B) % 256;
                } else {
                    final_r_bit = (final_r_bit + r_bit_C) % 256;
                }

                int g_bit_P = previous_g_bit + top_g_bit - top_left_g_bit;
                int g_bit_A = abs(g_bit_P - previous_g_bit);
                int g_bit_B = abs(g_bit_P - top_g_bit);
                int g_bit_C = abs(g_bit_P - top_left_g_bit);
                if (g_bit_A <= g_bit_B && g_bit_A <= g_bit_C) {
                    final_g_bit = (final_g_bit + g_bit_A) % 256;
                } else if (g_bit_B <= g_bit_C) {
                    final_g_bit = (final_g_bit + g_bit_B) % 256;
                } else {
                    final_g_bit = (final_g_bit + g_bit_C) % 256;
                }

                int b_bit_P = previous_b_bit + top_b_bit - top_left_b_bit;
                int b_bit_A = abs(b_bit_P - previous_b_bit);
                int b_bit_B = abs(b_bit_P - top_b_bit);
                int b_bit_C = abs(b_bit_P - top_left_b_bit);
                if (b_bit_A <= b_bit_B && b_bit_A <= b_bit_C) {
                    final_b_bit = (final_b_bit + b_bit_A) % 256;
                } else if (b_bit_B <= b_bit_C) {
                    final_b_bit = (final_b_bit + b_bit_B) % 256;
                } else {
                    final_b_bit = (final_b_bit + b_bit_C) % 256;
                }
            }

            int r_bit = final_r_bit;
            int g_bit = final_g_bit;
            int b_bit = final_b_bit;

            uncompressed[bit_index] = r_bit;
            uncompressed[bit_index + 1] = g_bit;
            uncompressed[bit_index + 2] = b_bit;

            pixels[pixels_position].rgb.r = r_bit;
            pixels[pixels_position].rgb.g = g_bit;
            pixels[pixels_position].rgb.b = b_bit;
            pixels[pixels_position].rgb.a = 255;

            pixels[pixels_position].col = height_index;
            pixels[pixels_position].row = (row_index - 1) / 4;
            pixels_position++;

            row_index += 3;
        }
        height_index += 1;
    }

    *pl = pixels_position;
}

void getFilterGrayscale(
    int uncompressed[], 
    size_t uncompressed_len,
    
    Pixel pixels[],
    size_t *pl,

    int image_height,
    int row_size
) {
    size_t pixels_position = *pl;

    int current_filter_method = 0;
    int height_index = 0;
    for(int i = 0; i < image_height; i++) {
        int row_index = 0;
        while (true) {
            int height_row_size = height_index * row_size;
            int bit_index = height_row_size + row_index;

            if (bit_index >= row_size * (height_index + 1)) break;
            if ((size_t)bit_index >= uncompressed_len) break;
            if (row_index == 0) {
                current_filter_method = uncompressed[bit_index];
                row_index += 1;
                continue;
            }

            if (current_filter_method == 0) { // NO FILTER
                int gray_scale = uncompressed[bit_index];
                pixels[pixels_position].rgb.r = gray_scale;
                pixels[pixels_position].rgb.g = gray_scale;
                pixels[pixels_position].rgb.b = gray_scale;
                pixels[pixels_position].rgb.a = 255;

                pixels[pixels_position].col = height_index;
                pixels[pixels_position].row = (row_index - 1) / 4;
                pixels_position++;

                row_index += 1;
                continue;
            }

            int u16_gray_scale = uncompressed[bit_index];

            if (current_filter_method == 1) {
                int previous_gray_scale_bit = 0;

                if (row_index >= 1) {
                    previous_gray_scale_bit = uncompressed[bit_index - 1];
                }

                u16_gray_scale = (u16_gray_scale + previous_gray_scale_bit) % 256;
            } else if (current_filter_method == 2) {
                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                }

                u16_gray_scale = (u16_gray_scale + top_gray_scale_bit) % 256;
            } else if (current_filter_method == 3) {
                int previous_gray_scale_bit = 0;

                if (row_index >= 1) {
                    previous_gray_scale_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                }

                u16_gray_scale = (u16_gray_scale + ((top_gray_scale_bit + previous_gray_scale_bit) / 2)) % 256;
            } else if (current_filter_method == 4) {
                int previous_gray_scale_bit = 0;

                if (row_index >= 1) {
                    previous_gray_scale_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                }

                int top_left_gray_scale_bit = 0;

                if (height_index > 0 && row_index >= 1) {
                    top_left_gray_scale_bit = uncompressed[bit_index - row_move_up - 1];
                }

                int gray_scale_bit_p = previous_gray_scale_bit + top_gray_scale_bit - top_left_gray_scale_bit;
                int gray_scale_bit_a = abs(gray_scale_bit_p - previous_gray_scale_bit);
                int gray_scale_bit_b = abs(gray_scale_bit_p - top_gray_scale_bit);
                int gray_scale_bit_c = abs(gray_scale_bit_p - top_left_gray_scale_bit);
                if (gray_scale_bit_a <= gray_scale_bit_b && gray_scale_bit_a <= gray_scale_bit_c) {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_a) % 256;
                } else if (gray_scale_bit_b <= gray_scale_bit_c) {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_b) % 256;
                } else {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_c) % 256;
                }
            }

            int gray_scale = u16_gray_scale;
            uncompressed[bit_index] = gray_scale;

            pixels[pixels_position].rgb.r = gray_scale;
            pixels[pixels_position].rgb.g = gray_scale;
            pixels[pixels_position].rgb.b = gray_scale;
            pixels[pixels_position].rgb.a = 255;

            pixels[pixels_position].col = height_index;
            pixels[pixels_position].row = (row_index - 1) / 4;
            pixels_position++;

            row_index += 1;
        }
        height_index += 1;
    }

    *pl = pixels_position;
}

void getFilterGrayscaleA(
    int uncompressed[], 
    size_t uncompressed_len,
    
    Pixel pixels[],
    size_t *pl,

    int image_height,
    int row_size
) {
    size_t pixels_position = *pl;

    int current_filter_method = 0;
    int height_index = 0;
    for (int i = 0; i <image_height; i++) {
        int row_index = 0;
        while (true) {
            int height_row_size = height_index * row_size;
            int bit_index = height_row_size + row_index;

            if (bit_index >= row_size * (height_index + 1)) break;
            if ((size_t)bit_index >= uncompressed_len) break;
            if (row_index == 0) {
                current_filter_method = uncompressed[bit_index];
                row_index += 1;
                continue;
            }


            if (current_filter_method == 0) { // NO FILTER
                int gray_scale = uncompressed[bit_index];
                int opacity_scale = uncompressed[bit_index + 1];
                pixels[pixels_position].rgb.r = gray_scale;
                pixels[pixels_position].rgb.g = gray_scale;
                pixels[pixels_position].rgb.b = gray_scale;
                pixels[pixels_position].rgb.a = opacity_scale;

                pixels[pixels_position].col = height_index;
                pixels[pixels_position].row = (row_index - 1) / 4;
                pixels_position++;

                row_index += 2;
                continue;
            }

            int u16_gray_scale = uncompressed[bit_index];
            int u16_opacity_scale = uncompressed[bit_index + 1];

            if (current_filter_method == 1) {
                int previous_gray_scale_bit = 0;
                int previous_opacity_scale_bit = 0;

                if (row_index >= 2) {
                    previous_gray_scale_bit = uncompressed[bit_index - 2];
                    previous_opacity_scale_bit = uncompressed[bit_index - 1];
                }

                u16_gray_scale = (u16_gray_scale + previous_gray_scale_bit) % 256;
                u16_opacity_scale = (u16_opacity_scale + previous_opacity_scale_bit) % 256;
            } else if (current_filter_method == 2) {
                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;
                int top_opacity_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                    top_opacity_scale_bit = uncompressed[bit_index + 1 - row_move_up];
                }

                u16_gray_scale = (u16_gray_scale + top_gray_scale_bit) % 256;
                u16_opacity_scale = (u16_opacity_scale + top_opacity_scale_bit) % 256;
            } else if (current_filter_method == 3) {
                int previous_gray_scale_bit = 0;
                int previous_opacity_scale_bit = 0;

                if (row_index >= 2) {
                    previous_gray_scale_bit = uncompressed[bit_index - 2];
                    previous_opacity_scale_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;
                int top_opacity_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                    top_opacity_scale_bit = uncompressed[bit_index + 1 - row_move_up];
                }

                u16_gray_scale = (u16_gray_scale + ((top_gray_scale_bit + previous_gray_scale_bit) / 2)) % 256;
                u16_opacity_scale = (u16_opacity_scale + ((top_opacity_scale_bit + previous_opacity_scale_bit) / 2)) % 256;
            } else if (current_filter_method == 4) {
                int previous_gray_scale_bit = 0;
                int previous_opacity_scale_bit = 0;

                if (row_index >= 2) {
                    previous_gray_scale_bit = uncompressed[bit_index - 2];
                    previous_opacity_scale_bit = uncompressed[bit_index - 1];
                }

                int row_move_up = height_row_size;
                int top_gray_scale_bit = 0;
                int top_opacity_scale_bit = 0;

                if (height_index > 0) {
                    top_gray_scale_bit = uncompressed[bit_index - row_move_up];
                    top_opacity_scale_bit = uncompressed[bit_index + 1 - row_move_up];
                }

                int top_left_gray_scale_bit = 0;
                int top_left_opacity_scale_bit = 0;

                if (height_index > 0 && row_index >= 2) {
                    top_left_gray_scale_bit = uncompressed[bit_index - row_move_up - 1];
                    top_left_opacity_scale_bit = uncompressed[bit_index + 1 - row_move_up - 1];
                }

                int gray_scale_bit_p = previous_gray_scale_bit + top_gray_scale_bit - top_left_gray_scale_bit;
                int gray_scale_bit_a = abs(gray_scale_bit_p - previous_gray_scale_bit);
                int gray_scale_bit_b = abs(gray_scale_bit_p - top_gray_scale_bit);
                int gray_scale_bit_c = abs(gray_scale_bit_p - top_left_gray_scale_bit);
                if (gray_scale_bit_a <= gray_scale_bit_b && gray_scale_bit_a <= gray_scale_bit_c) {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_a) % 256;
                } else if (gray_scale_bit_b <= gray_scale_bit_c) {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_b) % 256;
                } else {
                    u16_gray_scale = (u16_gray_scale + gray_scale_bit_c) % 256;
                }

                int opacity_scale_bit_p = previous_opacity_scale_bit + top_opacity_scale_bit - top_left_opacity_scale_bit;
                int opacity_scale_bit_a = abs(opacity_scale_bit_p - previous_opacity_scale_bit);
                int opacity_scale_bit_b = abs(opacity_scale_bit_p - top_opacity_scale_bit);
                int opacity_scale_bit_c = abs(opacity_scale_bit_p - top_left_opacity_scale_bit);
                if (opacity_scale_bit_a <= opacity_scale_bit_b && opacity_scale_bit_a <= opacity_scale_bit_c) {
                    u16_opacity_scale = (u16_opacity_scale + opacity_scale_bit_a) % 256;
                } else if (opacity_scale_bit_b <= opacity_scale_bit_c) {
                    u16_opacity_scale = (u16_opacity_scale + opacity_scale_bit_b) % 256;
                } else {
                    u16_opacity_scale = (u16_opacity_scale + opacity_scale_bit_c) % 256;
                }
            }

            int gray_scale = u16_gray_scale;
            int opacity_scale = u16_opacity_scale;

            uncompressed[bit_index] = gray_scale;

            pixels[pixels_position].rgb.r = gray_scale;
            pixels[pixels_position].rgb.g = gray_scale;
            pixels[pixels_position].rgb.b = gray_scale;
            pixels[pixels_position].rgb.a = opacity_scale;

            pixels[pixels_position].col = height_index;
            pixels[pixels_position].row = (row_index - 1) / 4;
            pixels_position++;

            row_index += 2;
        }
        height_index += 1;
    }

    *pl = pixels_position;
}


void getFilter(
    Pixel pixels[],
    size_t *pixels_position,

    int uncompressed[], 
    size_t uncompressed_len,

    PNG *png
) {
    size_t image_width = png->ihdr.width;
    size_t image_height = png->ihdr.height;
    
    // The row size is 1 + width*mult (mult up to 4) and is passed as an int.
    // Reject negative dimensions and widths large enough to overflow it.
    if (/* image_width < 0 || image_height < 0 || */ image_width > (__SIZE_MAX__ - 1) / 4) {
        return;
    }

    switch (png->ihdr.color_type) {
        case 0: {
            getFilterGrayscale(
                uncompressed, 
                uncompressed_len, 
                pixels,
                pixels_position,
                image_height, 
                (1 + (image_width * 1))
            );
            break;
        };
        case 2: {
            getFilterRgb(
                uncompressed, 
                uncompressed_len, 
                pixels, 
                pixels_position,
                image_height, 
                (1 + (image_width * 3))
            );
            break;
        };
        case 4: {
            getFilterGrayscaleA(
                uncompressed, 
                uncompressed_len, 
                pixels, 
                pixels_position,
                image_height, 
                (1 + (image_width * 2))
            );
            break;
        };
        case 6: {
            getFilterRgbA(
                uncompressed, 
                uncompressed_len, 
                pixels, 
                pixels_position,
                image_height, 
                (1 + (image_width * 4))
            );
            break;
        };
        default: {
            printf("invaliod ct type");
            exit(EXIT_FAILURE);
        }
    };
}
