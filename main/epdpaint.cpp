/**
 *  @filename   :   epdpaint.cpp
 *  @brief      :   Paint tools
 *  @author     :   Yehui from Waveshare
 *  
 *  Copyright (C) Waveshare     September 9 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of ept_paint software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "epdpaint.h"

void epd_paint_init(ept_paint_t *ept_paint, unsigned char *image, int width, int height) {
    ept_paint->rotate = ROTATE_0;
    ept_paint->image = image;
    /* 1 byte = 8 pixels, so the width should be the multiple of 8 */
    ept_paint->width = width % 8 ? width + 8 - (width % 8) : width;
    ept_paint->height = height;
}

void epd_paint_deinit() {

}

/**
 *  @brief: clear the image
 */
void epd_paint_clear(ept_paint_t *ept_paint, int colored) {
    for (int x = 0; x < ept_paint->width; x++) {
        for (int y = 0; y < ept_paint->height; y++) {
            ept_paint_draw_absolute_pixel(ept_paint, x, y, colored);
        }
    }
}

/**
 *  @brief: draws a pixel by absolute coordinates.
 *          this function won't be affected by the rotate parameter.
 */
void epd_paint_drawAbsolutePixel(ept_paint_t *ept_paint, int x, int y, int colored) {
    if (x < 0 || x >= ept_paint->width || y < 0 || y >= ept_paint->height) {
        return;
    }
    if (IF_INVERT_COLOR) {
        if (colored) {
            ept_paint->image[(x + y * ept_paint->width) / 8] |= 0x80 >> (x % 8);
        } else {
            ept_paint->image[(x + y * ept_paint->width) / 8] &= ~(0x80 >> (x % 8));
        }
    } else {
        if (colored) {
            ept_paint->image[(x + y * ept_paint->width) / 8] &= ~(0x80 >> (x % 8));
        } else {
            ept_paint->image[(x + y * ept_paint->width) / 8] |= 0x80 >> (x % 8);
        }
    }
}

/**
 *  @brief: ept_paint draws a pixel by the coordinates
 */
void epd_paint_draw_pixel(ept_paint_t *ept_paint, int x, int y, int colored) {
    int point_temp;
    if (ept_paint->rotate == ROTATE_0) {
        if (x < 0 || x >= ept_paint->width || y < 0 || y >= ept_paint->height) {
            return;
        }
        epd_paint_drawAbsolutePixel(ept_paint, x, y, colored);
    } else if (ept_paint->rotate == ROTATE_90) {
        if (x < 0 || x >= ept_paint->height || y < 0 || y >= ept_paint->width) {
            return;
        }
        point_temp = x;
        x = ept_paint->width - y;
        y = point_temp;
        epd_paint_drawAbsolutePixel(ept_paint, x, y, colored);
    } else if (ept_paint->rotate == ROTATE_180) {
        if (x < 0 || x >= ept_paint->width || y < 0 || y >= ept_paint->height) {
            return;
        }
        x = ept_paint->width - x;
        y = ept_paint->height - y;
        epd_paint_drawAbsolutePixel(ept_paint, x, y, colored);
    } else if (ept_paint->rotate == ROTATE_270) {
        if (x < 0 || x >= ept_paint->height || y < 0 || y >= ept_paint->width) {
            return;
        }
        point_temp = x;
        x = y;
        y = ept_paint->height - point_temp;
        epd_paint_drawAbsolutePixel(ept_paint, x, y, colored);
    }
}

/**
 *  @brief: draws a charactor on the frame buffer but not refresh
 */
void epd_paint_draw_char_at(ept_paint_t *ept_paint, int x, int y, char ascii_char, sFONT *font, int colored) {
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const uint8_t *ptr = &font->table[char_offset];

    for (j = 0; j < font->Height; j++) {
        for (i = 0; i < font->Width; i++) {
            if (*ptr & (0x80 >> (i % 8))) {
                epd_paint_draw_pixel(ept_paint, x + i, y + j, colored);
            }
            if (i % 8 == 7) {
                ptr++;
            }
        }
        if (font->Width % 8 != 0) {
            ptr++;
        }
    }
}

/**
*  @brief: ept_paint displays a string on the frame buffer but not refresh
*/
void epd_paint_draw_string_at(ept_paint_t *ept_paint, int x, int y, const char *text, sFONT *font, int colored) {
    const char *p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;

    /* Send the string character by character on EPD */
    while (*p_text != 0) {
        /* Display one character on EPD */
        epd_paint_draw_char_at(ept_paint, refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->Width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
}

/**
*  @brief: draws a line on the frame buffer
*/
void epd_paint_draw_line(ept_paint_t *ept_paint, int x0, int y0, int x1, int y1, int colored) {
    /* Bresenham algorithm */
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while ((x0 != x1) && (y0 != y1)) {
        epd_paint_draw_pixel(ept_paint, x0, y0, colored);
        if (2 * err >= dy) {
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
*  @brief: ept_paint draws a horizontal line on the frame buffer
*/
void epd_paint_draw_horizontal_line(ept_paint_t *ept_paint, int x, int y, int line_width, int colored) {
    int i;
    for (i = x; i < x + line_width; i++) {
        epd_paint_draw_pixel(ept_paint, i, y, colored);
    }
}

/**
*  @brief: draws a vertical line on the frame buffer
*/
void epd_paint_draw_vertical_line(ept_paint_t *ept_paint, int x, int y, int line_height, int colored) {
    int i;
    for (i = y; i < y + line_height; i++) {
        epd_paint_draw_pixel(ept_paint, x, i, colored);
    }
}


void epd_paint_draw_rectangle(ept_paint_t *ept_paint, int x0, int y0, int x1, int y1, int colored) {
    int min_x, min_y, max_x, max_y;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    epd_paint_draw_horizontal_line(ept_paint, min_x, min_y, max_x - min_x + 1, colored);
    epd_paint_draw_horizontal_line(ept_paint, min_x, max_y, max_x - min_x + 1, colored);
    epd_paint_draw_vertical_line(ept_paint, min_x, min_y, max_y - min_y + 1, colored);
    epd_paint_draw_vertical_line(ept_paint, max_x, min_y, max_y - min_y + 1, colored);
}

/**
*  @brief: ept_paint draws a filled rectangle
*/
void epd_paint__draw_filled_rectangle(ept_paint_t *ept_paint, int x0, int y0, int x1, int y1, int colored) {
    int min_x, min_y, max_x, max_y;
    int i;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;

    for (i = min_x; i <= max_x; i++) {
        epd_paint_draw_vertical_line(ept_paint, i, min_y, max_y - min_y + 1, colored);
    }
}

void epd_paint_draw_circle(ept_paint_t *ept_paint, int x, int y, int radius, int colored) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        epd_paint_draw_pixel(ept_paint, x - x_pos, y + y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x + x_pos, y + y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x + x_pos, y - y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x - x_pos, y - y_pos, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

/**
*  @brief: draws a filled circle
*/
void epd_paint_draw_filled_circle(ept_paint_t *ept_paint, int x, int y, int radius, int colored) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        epd_paint_draw_pixel(ept_paint, x - x_pos, y + y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x + x_pos, y + y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x + x_pos, y - y_pos, colored);
        epd_paint_draw_pixel(ept_paint, x - x_pos, y - y_pos, colored);
        epd_paint_draw_horizontal_line(ept_paint, x + x_pos, y + y_pos, 2 * (-x_pos) + 1, colored);
        epd_paint_draw_horizontal_line(ept_paint, x + x_pos, y - y_pos, 2 * (-x_pos) + 1, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if (-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

/* END OF FILE */























