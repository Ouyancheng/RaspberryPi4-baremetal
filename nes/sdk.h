#pragma once 
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
#   define PLATFORM_UNIX 1 
#endif
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#ifndef PLATFORM_UNIX
#   include "c_string.h"
#   include "printf.h"
#   include "delay.h"
#else 
#   include <string.h>
#   include <stdio.h>
#   include <unistd.h>
static inline void delay_us(unsigned us) {
    usleep(us); 
}
static inline void delay_ms(unsigned ms) {
    delay_us(ms * 1000); 
}
static inline void delay_sec(unsigned sec) {
    sleep(sec); 
}
int panic_at(const char *file, int line, const char *function, const char *format, ...); 
#define panic(...) panic_at(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define assert(condition) ((condition) ? ((void)0) : ((void)panic("assertion " #condition " failed"))) 

#endif 





