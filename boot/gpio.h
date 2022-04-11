#pragma once 
#include <stdint.h>
#include "boot.h"

#define GPFSEL0         ((volatile unsigned int*)(MMIO_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(MMIO_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(MMIO_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(MMIO_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(MMIO_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(MMIO_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(MMIO_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(MMIO_BASE+0x00200028))
#define GPCLR1          ((volatile unsigned int*)(MMIO_BASE+0x0020002C))
#define GPLEV0          ((volatile unsigned int*)(MMIO_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(MMIO_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(MMIO_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(MMIO_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(MMIO_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(MMIO_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(MMIO_BASE+0x0020009C))
// rpi 4
#define GPPUD0          ((volatile unsigned int*)(MMIO_BASE+0x002000e4))
#define GPPUD1          ((volatile unsigned int*)(MMIO_BASE+0x002000e8))
#define GPPUD2          ((volatile unsigned int*)(MMIO_BASE+0x002000ec))
#define GPPUD3          ((volatile unsigned int*)(MMIO_BASE+0x002000f0))


#define GPIO_CHECK_INVALID(p) ((p) >= 54)

typedef enum {
    GPIO_FUNC_INPUT   = 0,
    GPIO_FUNC_OUTPUT  = 1,
    GPIO_FUNC_ALT0    = 4,
    GPIO_FUNC_ALT1    = 5,
    GPIO_FUNC_ALT2    = 6,
    GPIO_FUNC_ALT3    = 7,
    GPIO_FUNC_ALT4    = 3,
    GPIO_FUNC_ALT5    = 2,
} gpio_func_t;
typedef enum {
    GPIO_PULL_NONE = 0b00,
    GPIO_PULL_UP   = 0b01,
    GPIO_PULL_DOWN = 0b10,
} gpio_pull_t;

#ifdef __cplusplus
extern "C" {
#endif 
void gpio_set_function(unsigned pin, gpio_func_t function);
int gpio_get_function(unsigned pin);
void gpio_set_on(unsigned pin);
void gpio_set_off(unsigned pin);
void gpio_set(unsigned pin, int v);
int gpio_read(unsigned pin);
void gpio_set_pull(unsigned pin, gpio_pull_t pull); 
#ifdef __cplusplus
}
#endif 

