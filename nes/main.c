#include "sdk.h"
#include "cpu.h"
#include "cpu_debug.h"
#ifdef PLATFORM_UNIX 
#include "SDL2/SDL.h"
#include <stdbool.h>
#include <stdlib.h>
#include "pputest.h"
#include <signal.h>
#define SDL_DRAW 1
uint8_t snake_game[] = {
    0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
    0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
    0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
    0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
    0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 
    0xa5, 
    0xff, 
    0xc9, 0x77, 
    0xf0, 0x0d, 
    0xc9, 0x64, 
    0xf0, 0x14, 
    0xc9, 0x73, 
    0xf0, 0x1b, 
    0xc9, 0x61, 
    0xf0, 0x22, 
    0x60,
    0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
    0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
    0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
    0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
    0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
    0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
    0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
    0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
    0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
    0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
    0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
    0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
    0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
    0xea, 0xca, 0xd0, 0xfb, 0x60
};
SDL_Window* win;
SDL_Renderer *renderer; 
SDL_Surface *surface; 
SDL_Texture *texture; 
uint8_t *pixels;
int pitch;
void cleanup_and_exit(int code) {
    SDL_DestroyTexture(texture); 
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(win);
    SDL_Quit();
    exit(code); 
}
void handle_input(void) {
    SDL_Event e;
    bool quit = false;
    int ret = SDL_PollEvent(&e); 
    
    if (!ret) { return; }
    if (e.type == SDL_QUIT) {
        cleanup_and_exit(0); 
    }
    if (e.type == SDL_KEYDOWN) {
        // SDL_KeyboardEvent key = e.key; 
        // SDL_Keysym keysym = key.keysym; 
        SDL_Keycode keycode = e.key.keysym.sym; 
        switch (keycode) {
            case SDLK_w: 
                cpu_mem_write(0xff, 0x77); 
                break; 
            case SDLK_a: 
                cpu_mem_write(0xff, 0x61); 
                break; 
            case SDLK_s: 
                cpu_mem_write(0xff, 0x73); 
                break; 
            case SDLK_d: 
                cpu_mem_write(0xff, 0x64); 
                break; 
            case SDLK_ESCAPE: 
            case SDLK_q: 
                cleanup_and_exit(0); 
                break; 
            default: 
                break; 
        }
    }
}
struct rgb get_color(uint8_t b) {
    struct rgb color; 
    color.a = SDL_ALPHA_OPAQUE; 
    switch (b) {
    case 0: 
        color.r = 0;
        color.g = 0; 
        color.b = 0; 
        break; 
    case 1: 
        color.r = 0xff; 
        color.g = 0xff; 
        color.b = 0xff; 
        break; 
    case 2: 
    case 9: 
        color.r = 0x6f; 
        color.g = 0x6f; 
        color.b = 0x6f; 
        break; 
    case 3: 
    case 10: 
        color.r = 0xff; 
        color.g = 0x0; 
        color.b = 0x0; 
        break; 
    case 4: 
    case 11: 
        color.r = 0x0; 
        color.g = 0xff; 
        color.b = 0x0; 
        break; 
    case 5: 
    case 12: 
        color.r = 0x0; 
        color.g = 0x0; 
        color.b = 0xff; 
        break; 
    case 6: 
    case 13: 
        color.r = 0xff; 
        color.g = 0x0; 
        color.b = 0xff; 
        break; 
    case 7: 
    case 14: 
        color.r = 0xff; 
        color.g = 0xff; 
        color.b = 0x0; 
        break; 
    default: 
        color.r = 0x0; 
        color.g = 0xff; 
        color.b = 0xff; 
        break; 
    }
    return color; 
}
struct rgb framebuffer[32*32]; 
int read_screen_state(void) {
    int update = 0; 
    unsigned x; 
    unsigned y; 
    unsigned idx; 
    for (uint16_t i = 0x0200; i < 0x0600; ++i) {
        // 512 - 1535 
        // 1536 - 512 = 1024 
        uint8_t color_idx = cpu_mem_read(i); 
        // if (color_idx == 0) {continue; }
        struct rgb color = get_color(color_idx); 
        idx = i-0x0200; 
        y = idx / 32; 
        x = idx - (32 * y); 
        #if SDL_DRAW
        // SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE); 
        // SDL_RenderDrawPoint(renderer, x, y); 
        framebuffer[idx] = color; 
        // pixels[idx * 4] = color.a; 
        // pixels[idx * 4 + 1] = color.b; 
        // pixels[idx * 4 + 2] = color.g; 
        // pixels[idx * 4 + 3] = color.r; 
        // pixels[y*pitch + 4*x]   = color.a;
        // pixels[y*pitch + 4*x+1] = color.b;
        // pixels[y*pitch + 4*x+2] = color.g;
        // pixels[y*pitch + 4*x+3] = color.r;
        #endif 
    }
    return update; 
}
int cnt = 0; 
uint8_t random_number = 0; 
void callback(void) {
    cnt += 1; 
    if (cnt % 600 == 0) {
        // printf("cnt=%d\n", cnt); 
        // cnt = 0; 
        #if SDL_DRAW
        if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch) != 0) {
            panic("failed to lock texture!\n"); 
        } 
        memcpy(pixels, framebuffer, 32*32*4); 
        // for (int y = 0; y < 32; ++y) {
        //     for (int x = 0; x < 32; ++x) {
        //         pixels[y*pitch + 4*x]   = framebuffer[y*32+x].a;
        //         pixels[y*pitch + 4*x+1] = framebuffer[y*32+x].b;
        //         pixels[y*pitch + 4*x+2] = framebuffer[y*32+x].g;
        //         pixels[y*pitch + 4*x+3] = framebuffer[y*32+x].r;
        //     }
        // }
        SDL_UnlockTexture(texture); 
        if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0) {
            panic("failed to copy texture\n"); 
        }
        SDL_RenderPresent(renderer); 
        
        SDL_Delay(42); 
        #endif 
    }
    handle_input(); 
    random_number = (random_number + 1) % 16; 
    cpu_mem_write(0xfe, random_number);
    read_screen_state(); 
    
}

void test_sdl(void) {
#if TEST_SDL 
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    // SDL_ShowWindow(win); 
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = true;
            }
        }
    }
    // SDL_Delay(10000);

    SDL_DestroyWindow(win);
    SDL_Quit();
#endif 
}
void test_cpu_rom(void) {
    srand(0); 
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        panic("error initializing SDL: %s\n", SDL_GetError());
    }
    win = SDL_CreateWindow(
        "GAME",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        32*10, 32*10, 0
    );
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal"); 
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderSetScale(renderer, 10.0, 10.0); 
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 32, 32); 
    if (!texture) {
        panic("failed to get texture!\n"); 
    }
    SDL_SetRenderTarget(renderer, texture);
    // if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch) != 0) {
    //     panic("failed to lock texture!\n"); 
    // } 

    // cpu_init(); 
    // cpu_load_program((uint8_t*)snake_game, sizeof(snake_game)); 
    // cpu_reset(); 
    // cpu_run_with_callback(callback); 

    FILE *romfile = fopen("../testroms/snake.nes", "rb"); 
    // FILE *romfile = fopen("../testroms/nestest.nes", "rb"); 
    if (!romfile) {
        panic("romfile not found!\n"); 
    }
    size_t romsize; 
    fseek(romfile , 0L , SEEK_END);
    romsize = ftell(romfile);
    rewind(romfile); 
    if (fread((uint8_t*)rom_bytes, romsize, 1, romfile) != 1) {
        panic("rom file read failed!\n"); 
    }
    fclose(romfile); 

    struct nes_rom rom = load_rom((uint8_t*)rom_bytes, romsize); 
    // printf("romsize = %zu, prgsize = %zu, chrsize = %zu\n", romsize, rom.prg_rom_size, rom.chr_rom_size); 
    bus_init(rom); 
    cpu_init(); 
    cpu_reset(); 
    
    cpu_run_with_callback(callback); 

    // cpu.pc = 0xC000; // hack 
    // cpu_run_with_callback(dump_cpu); 
}
int main(void) {
    test_cpu_rom(); 
    printf("Done!!!\n"); 
    return 0; 
}

#endif 
