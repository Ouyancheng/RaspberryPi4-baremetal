#pragma once 
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__)
#   define PLATFORM_UNIX 1 
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
#   define PLATFORM_WINDOWS 1
#endif 
#if defined(PLATFORM_UNIX) || defined(PLATFORM_WINDOWS)
#   define PLATFORM_PC 1
#endif 
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#ifdef PLATFORM_RPI
#   include "common.h"
#   include "c_string.h"
#   include "printf.h"
#   include "delay.h"
#   include "crc.h"
#   include "framebuffer.h"
#   include "gpio.h"
#   include "mbox.h"
#   include "uart.h"
#   include "uartgetcode.h"
#   include "util.h"
static inline void exit(int code) {
    rpi_reboot(); 
} 
static inline unsigned get_current_time_ms(void) {
    return get_current_time_us() / 1000; 
}
#endif 

#ifdef PLATFORM_PC  
#   include <string.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <signal.h>
#   include <SDL2/SDL.h>

static inline void delay_ms(unsigned ms) {
    SDL_Delay(ms); 
}
static inline void delay_sec(unsigned sec) {
    for (unsigned i = 0; i < sec; ++i) {
        delay_ms(1000); 
    }
}
static inline unsigned get_current_time_ms(void) {
    return SDL_GetTicks(); 
}

#endif 
int panic_at(const char *file, int line, const char *function, const char *format, ...); 
#define panic(...) panic_at(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define assert(condition) ((condition) ? ((void)0) : ((void)panic("assertion " #condition " failed"))) 


struct rgb {
    uint8_t a; 
    uint8_t r; 
    uint8_t g; 
    uint8_t b; 
};
static inline uint8_t set_mask(uint8_t reg, uint8_t mask) {
    reg |= mask; 
    return reg; 
}
static inline uint8_t clear_mask(uint8_t reg, uint8_t mask) {
    reg &= (~mask); 
    return reg; 
}
#define set_mask_inplace(reg, mask) ((reg) |= ((uint8_t)(mask)))
#define clear_mask_inplace(reg, mask) ((reg) &= (~((uint8_t)(mask))))
void do_nothing(void);



unsigned load_file(uint8_t *target, unsigned target_size, const char *filename); 

#define FRAMERATE 60 

