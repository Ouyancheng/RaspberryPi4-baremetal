#include "controller_interface.h"
#include "controller.h"
#include "display_interface.h"


#ifdef PLATFORM_PC 
#include <SDL2/SDL.h> 
void controller_handle_input(unsigned timeout) {
    SDL_Event e;
    bool quit = false;
    int ret; 
    int event_cnt = 0; 
    do {
        if (timeout == 0) {
            ret = SDL_PollEvent(&e); 
        } else {
            ret = SDL_WaitEventTimeout(&e, (int)timeout); 
        }
        if (!ret) { break; }
        event_cnt += 1; 
        if (e.type == SDL_QUIT) {
            display_exit(); 
            exit(0); 
        }
        // printf("e.type = %d\n", e.type); 
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            SDL_Keycode keycode = e.key.keysym.sym; 
            bool set_key = e.type == SDL_KEYDOWN; 
            switch (keycode) {
            case SDLK_w: 
                controller_latch_button(&controller1, controller_button_up, set_key); 
                // controller_latch_button(&controller2, controller_button_up, set_key); 
                break; 
            case SDLK_a: 
                controller_latch_button(&controller1, controller_button_left, set_key); 
                // controller_latch_button(&controller2, controller_button_left, set_key); 
                break; 
            case SDLK_s: 
                controller_latch_button(&controller1, controller_button_down, set_key); 
                // controller_latch_button(&controller2, controller_button_down, set_key); 
                break; 
            case SDLK_d: 
                controller_latch_button(&controller1, controller_button_right, set_key); 
                // controller_latch_button(&controller2, controller_button_right, set_key); 
                break; 
            case SDLK_j:
                controller_latch_button(&controller1, controller_button_button_b, set_key);  
                // controller_latch_button(&controller2, controller_button_button_b, set_key);  
                break; 
            case SDLK_k: 
                controller_latch_button(&controller1, controller_button_button_a, set_key);  
                // controller_latch_button(&controller2, controller_button_button_a, set_key);  
                break; 
            case SDLK_SPACE: 
                controller_latch_button(&controller1, controller_button_start, set_key);  
                // controller_latch_button(&controller2, controller_button_start, set_key);  
                break; 
            case SDLK_TAB: 
                controller_latch_button(&controller1, controller_button_select, set_key);  
                // controller_latch_button(&controller2, controller_button_select, set_key);  
                break; 
            case SDLK_ESCAPE: 
            case SDLK_q: 
                if (e.type == SDL_KEYDOWN) {
                    display_exit(); 
                    exit(0); 
                }
                break; 
            default: 
                break; 
            }
        }
    } while (timeout == 0); 
}
#endif 



#ifdef PLATFORM_RPI 
void controller_handle_input(unsigned timeout) {
    // int hasdata = uart_has_data(UART0_BASE); 
    // if (!hasdata) {
    //     return; 
    // }
    char c = '\0';
    asm volatile("nop":::"memory"); 
    dev_barrier(); 
    // while (uart_has_data(UART0_BASE)) {
    //     asm volatile("nop":::"memory"); 
    //     dev_barrier(); 
    //     c = uart_getc(UART0_BASE); 
    // }
    asm volatile("nop":::"memory"); 
    dev_barrier(); 
    while (uart_has_data(UART0_BASE)) {
        // printf("%c", c); 
        asm volatile("nop":::"memory"); 
        dev_barrier(); 
        c = uart_getc(UART0_BASE); 
        bool set_key = true; 
        switch (c) {
            case 'w': 
                controller_latch_button(&controller1, controller_button_up, set_key); 
                // controller_latch_button(&controller2, controller_button_up, set_key); 
                break; 
            case 'a': 
                controller_latch_button(&controller1, controller_button_left, set_key); 
                // controller_latch_button(&controller2, controller_button_left, set_key); 
                break; 
            case 's': 
                controller_latch_button(&controller1, controller_button_down, set_key); 
                // controller_latch_button(&controller2, controller_button_down, set_key); 
                break; 
            case 'd': 
                controller_latch_button(&controller1, controller_button_right, set_key); 
                // controller_latch_button(&controller2, controller_button_right, set_key); 
                break; 
            case 'j':
                controller_latch_button(&controller1, controller_button_button_b, set_key);  
                // controller_latch_button(&controller2, controller_button_button_b, set_key);  
                break; 
            case 'k': 
                controller_latch_button(&controller1, controller_button_button_a, set_key);  
                // controller_latch_button(&controller2, controller_button_button_a, set_key);  
                break; 
            case ' ': 
                controller_latch_button(&controller1, controller_button_start, set_key);  
                // controller_latch_button(&controller2, controller_button_start, set_key);  
                break; 
            case '\t': 
                controller_latch_button(&controller1, controller_button_select, set_key);  
                // controller_latch_button(&controller2, controller_button_select, set_key);  
                break; 
            case 'q': 
                display_exit(); 
                exit(0); 
                break; 
            default: 
                break; 
        }
    }
    asm volatile("nop":::"memory"); 
    dev_barrier(); 
    
}
#endif 



