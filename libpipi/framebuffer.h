#pragma once 
#include <stdint.h>
// pixel order is R,G,B 
extern unsigned char* fb; 
extern unsigned int width, height, pitch, isrgb; 
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
static inline void draw_pixel_rgba(unsigned char *buffer, int x, int y, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    int offs = (y * pitch) + (x * 4); 
    *((uint32_t*)(buffer + offs)) = ((a << 24) | (r << 16) | (g << 8) | b); 
}
void draw_char(unsigned char ch, int x, int y, unsigned char attr);
void draw_string(int x, int y, char *s, unsigned char attr);
void draw_rectangle(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
void draw_rectangle_rgba(unsigned char *buffer, int x1, int y1, int x2, int y2, uint32_t r, uint32_t g, uint32_t b, uint32_t a, int fill); 
void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned char attr);
void draw_line_rgba(unsigned char *buffer, int x1, int y1, int x2, int y2, uint32_t r, uint32_t g, uint32_t b, uint32_t a); 
