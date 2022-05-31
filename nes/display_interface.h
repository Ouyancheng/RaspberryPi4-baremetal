#pragma once 

#include "sdk.h"


#define NES_DISPLAY_WIDTH 256 
#define NES_DISPLAY_HEIGHT 240 
#ifdef PLATFORM_PC 
#include "SDL2/SDL.h"
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
#endif 

void display_init(const char *const title, int width, int height, int scaling_w, int scaling_h); 
void display_exit(void); 
void display_set_pixel(unsigned x, unsigned y, struct rgb color); 
struct rgb display_get_pixel(unsigned x, unsigned y); 
void display_render_frame(void); 
void display_delay_for_framerate(void); 
extern uint32_t last_frame_tick; 




