/**
 * Code stolen from:
 * https://github.com/isometimes/rpi4-osdev/tree/master/part5-framebuffer
 */
#include "framebuffer.h"
#include "mbox.h"
#include "terminal.h"
unsigned int width, height, pitch, isrgb;
unsigned char* fb;
int framebuffer_init(uint32_t physical_width,
                     uint32_t physical_height,
                     uint32_t virtual_width,
                     uint32_t virtual_height,
                     uint32_t virtual_offset_width,
                     uint32_t virtual_offset_height,
                     uint32_t bits_per_pixel) {
    mbox[0] = 35 * 4;  // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SETPHYWH;  // Tag identifier
    mbox[3] = 8;                  // Value size in bytes
    mbox[4] = 0;
    mbox[5] = physical_width;   // Value(width)
    mbox[6] = physical_height;  // Value(height)

    mbox[7] = MBOX_TAG_SETVIRTWH;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = virtual_width;
    mbox[11] = virtual_height;

    mbox[12] = MBOX_TAG_SETVIRTOFF;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = virtual_offset_width;   // Value(x)
    mbox[16] = virtual_offset_height;  // Value(y)

    mbox[17] = MBOX_TAG_SETDEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = bits_per_pixel;  // Bits per pixel

    mbox[21] = MBOX_TAG_SETPXLORDR;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;  // RGB

    mbox[25] = MBOX_TAG_GETFB;
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;  // FrameBufferInfo.pointer
    mbox[29] = 0;     // FrameBufferInfo.size

    mbox[30] = MBOX_TAG_GETPITCH;
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;  // Bytes per line

    mbox[34] = MBOX_TAG_LAST;

    // Check call is successful and we have a pointer with depth 32
    if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF;  // Convert GPU address to ARM address
        width = mbox[10];        // Actual physical width
        height = mbox[11];       // Actual physical height
        pitch = mbox[33];        // Number of bytes per line
        isrgb = mbox[24];        // Pixel order
        fb = (unsigned char*)((long)mbox[28]);
        return 0;
    }
    return -1;
}

void draw_pixel(int x, int y, unsigned char attr) {
    int offs = (y * pitch) + (x * 4);
    *((unsigned int*)(fb + offs)) = vgapal[attr & 0x0f];
}

void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char attr, int fill) {
    int y = y1;

    while (y <= y2) {
        int x = x1;
        while (x <= x2) {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                draw_pixel(x, y, attr);
            else if (fill)
                draw_pixel(x, y, (attr & 0xf0) >> 4);
            x++;
        }
        y++;
    }
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char attr) {
    int dx, dy, p, x, y;

    dx = x2 - x1;
    dy = y2 - y1;
    x = x1;
    y = y1;
    p = 2 * dy - dx;

    while (x < x2) {
        if (p >= 0) {
            draw_pixel(x, y, attr);
            y++;
            p = p + 2 * dy - 2 * dx;
        } else {
            draw_pixel(x, y, attr);
            p = p + 2 * dy;
        }
        x++;
    }
}

void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        if (fill) {
            draw_line(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
            draw_line(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
            draw_line(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
            draw_line(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
        }
        draw_pixel(x0 - y, y0 + x, attr);
        draw_pixel(x0 + y, y0 + x, attr);
        draw_pixel(x0 - x, y0 + y, attr);
        draw_pixel(x0 + x, y0 + y, attr);
        draw_pixel(x0 - x, y0 - y, attr);
        draw_pixel(x0 + x, y0 - y, attr);
        draw_pixel(x0 - y, y0 - x, attr);
        draw_pixel(x0 + y, y0 - x, attr);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }

        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void draw_char(unsigned char ch, int x, int y, unsigned char attr) {
    unsigned char* glyph =
        (unsigned char*)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i = 0; i < FONT_HEIGHT; i++) {
        for (int j = 0; j < FONT_WIDTH; j++) {
            unsigned char mask = 1 << j;
            unsigned char col =
                (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

            draw_pixel(x + j, y + i, col);
        }
        glyph += FONT_BPL;
    }
}

void draw_string(int x, int y, char* s, unsigned char attr) {
    while (*s) {
        if (*s == '\r') {
            x = 0;
        } else if (*s == '\n') {
            x = 0;
            y += FONT_HEIGHT;
        } else {
            draw_char(*s, x, y, attr);
            x += FONT_WIDTH;
        }
        s++;
    }
}
