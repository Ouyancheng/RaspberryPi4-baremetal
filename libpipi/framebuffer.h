#pragma once 
#include <stdint.h>
// pixel order is R,G,B 
int framebuffer_init(
    uint32_t physical_width, 
    uint32_t physical_height, 
    uint32_t virtual_width, 
    uint32_t virtual_height,
    uint32_t virtual_offset_width, 
    uint32_t virtual_offset_height, 
    uint32_t bits_per_pixel 
);
void draw_pixel(int x, int y, unsigned char attr);
void draw_char(unsigned char ch, int x, int y, unsigned char attr);
void draw_string(int x, int y, char *s, unsigned char attr);
void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned char attr);
