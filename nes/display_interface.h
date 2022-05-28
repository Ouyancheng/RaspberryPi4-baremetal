#pragma once 

#include "sdk.h"
#include "ppu.h"
#ifndef PLATFORM_UNIX 
#error "not yet supported" 
#endif 

#include "SDL2/SDL.h"

#define NES_DISPLAY_WIDTH 256 
#define NES_DISPLAY_HEIGHT 240 

struct display_device {
    SDL_Window   *window;
    SDL_Renderer *renderer; 
    SDL_Surface  *surface; 
    SDL_Texture  *texture; 
    uint8_t      *pixels;
    int          pitch;
    int          width; 
    int          height; 
    uint32_t     last_tick; 
    struct rgb   framebuffer[NES_DISPLAY_WIDTH*NES_DISPLAY_HEIGHT+1]; 
};

extern struct display_device display; 


void display_init(const char *const title, int width, int height, int scaling_w, int scaling_h); 
void display_exit(void); 
void display_set_pixel(unsigned x, unsigned y, struct rgb color); 
void display_render_frame(void); 






