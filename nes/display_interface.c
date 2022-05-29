#include "display_interface.h"

uint32_t last_frame_tick; 


#ifdef PLATFORM_PC 
struct display_device display; 
void display_init(const char *const title, int width, int height, int scaling_w, int scaling_h) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        panic("error initializing SDL: %s\n", SDL_GetError());
    }
    display.window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * scaling_w, 
        height * scaling_h, 
        0
    );
    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal"); 
    // display.renderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_ACCELERATED);
    display.renderer = SDL_CreateRenderer(display.window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(display.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderSetScale(display.renderer, scaling_w, scaling_h); 
    SDL_RenderClear(display.renderer);
    SDL_RenderPresent(display.renderer);
    display.texture = SDL_CreateTexture(
        display.renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        width, 
        height
    ); 
    if (!display.texture) {
        panic("failed to get texture!\n"); 
    }
    SDL_SetRenderTarget(display.renderer, display.texture);
    display.pixels = 0; 
    display.pitch = 0; 
    display.surface = 0; 
    display.width = width; 
    display.height = height; 
    display.last_tick = 0; 
    memset((uint8_t*)display.framebuffer, 0, sizeof(struct rgb) * NES_DISPLAY_WIDTH * NES_DISPLAY_HEIGHT); 
}

void display_exit(void) {
    SDL_DestroyTexture(display.texture); 
    SDL_DestroyRenderer(display.renderer); 
    SDL_DestroyWindow(display.window);
    SDL_Quit();
}

void display_set_pixel(unsigned x, unsigned y, struct rgb color) {
    ////// MUST DO THIS!!!!!! Otherwise there are games that write out of bound!!!!!! 
    if (x < display.width && y < display.height) {
        display.framebuffer[y*display.width + x] = color; 
    }
}

void display_render_frame(void) {
    uint8_t *pixels;
    int pitch; 
    if (SDL_LockTexture(display.texture, NULL, (void**)&(pixels), &(pitch)) != 0) {
        panic("failed to lock texture!\n"); 
    } 
    for (int y = 0; y < display.height; ++y) {
        for (int x = 0; x < display.width; ++x) {
            pixels[y*pitch + 4*x]   = SDL_ALPHA_OPAQUE; //display.framebuffer[y*display.width+x].a;
            pixels[y*pitch + 4*x+1] = display.framebuffer[y*display.width+x].b;
            pixels[y*pitch + 4*x+2] = display.framebuffer[y*display.width+x].g;
            pixels[y*pitch + 4*x+3] = display.framebuffer[y*display.width+x].r;
        }
    }
    SDL_UnlockTexture(display.texture); 
    if (SDL_RenderCopy(display.renderer, display.texture, NULL, NULL) != 0) {
        panic("failed to copy texture\n"); 
    }
    SDL_RenderPresent(display.renderer); 
    memset((uint8_t*)display.framebuffer, 0, sizeof(struct rgb) * NES_DISPLAY_WIDTH * NES_DISPLAY_HEIGHT); 
}
#endif 



#ifdef PLATFORM_RPI 
void display_init(const char *const title, int width, int height, int scaling_w, int scaling_h) {
    dev_barrier(); 
    asm volatile ("nop":::"memory"); 
    int ret = framebuffer_init(NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT, 32, 0); 
    asm volatile ("nop":::"memory"); 
    printf("width=%d, height=%d pitch=%d\n", width, height, pitch); 
    asm volatile ("nop":::"memory"); 
    if (ret != 0) {
        printf("failed to initiate framebuffer! ret=%d \n\n", ret); 
        asm volatile ("nop":::"memory"); 
        dev_barrier(); 
        panic("!"); 
    }
    asm volatile ("nop":::"memory"); 
}
void display_exit(void) {

}
void display_set_pixel(unsigned x, unsigned y, struct rgb color) {
    asm volatile ("nop":::"memory"); 
    draw_pixel_rgba((uint8_t*)fb, x, y, color.r, color.g, color.b, 0xff); 
    asm volatile ("nop":::"memory"); 
}
void display_render_frame(void) {

}
#endif 






















