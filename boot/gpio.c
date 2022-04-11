#include "gpio.h"
#include "delay.h"
void gpio_set_function(unsigned pin, gpio_func_t function) {
    if (GPIO_CHECK_INVALID(pin)) return; 
    if ((int)function > 7 || (int)function < 0) return;
    uint32_t fn = pin / 10;
    volatile unsigned int *fsel = GPFSEL0 + fn; 
    uint32_t prev_fsel_val = *fsel; 
    uint32_t p = pin % 10;
    prev_fsel_val &= (~(0x7 << (p * 3)));
    prev_fsel_val |= (((int)function) << (p * 3));
    (*fsel) = prev_fsel_val; 
}

int gpio_get_function(unsigned pin) {
    if (GPIO_CHECK_INVALID(pin)) return -1;
    uint32_t fn = pin / 10;
    volatile unsigned int *fsel = GPFSEL0 + fn; 
    uint32_t prev_fsel_val = *fsel;
    uint32_t p = pin % 10;
    prev_fsel_val &= (0x7 << (p * 3));
    return (int)(prev_fsel_val >> (p * 3));
}

void gpio_set_on(unsigned pin) {
    uint32_t mask = 0;
    if (GPIO_CHECK_INVALID(pin)) return;
    if (pin <= 31) {
        mask = (1 << pin);
        (*GPSET0) = mask; 
    } 
    else {
        pin -= 32;
        mask = (1 << pin);
        (*GPSET1) = mask; 
    }
    return;
}

void gpio_set_off(unsigned pin) {
    uint32_t mask = 0;
    if (GPIO_CHECK_INVALID(pin)) return;
    if (pin <= 31) {
        mask = (1 << pin);
        (*GPCLR0) = mask; 
    } else {
        pin -= 32;
        mask = (1 << pin);
        (*GPCLR1) = mask; 
    }
    return;
}

void gpio_set(unsigned pin, int v) {
    if (v) gpio_set_on(pin); 
    else gpio_set_off(pin); 
}
int gpio_read(unsigned pin) {
    int v = 0;
    uint32_t mask = 0;
    uint32_t gpio_val = 0;
    if (GPIO_CHECK_INVALID(pin)) return -1; 
    if (pin <= 31) {
        mask = (1 << pin);
        gpio_val = *GPLEV0;
        v = !!(gpio_val & mask);
    } else {
        pin -= 32;
        mask = (1 << pin);
        gpio_val = *GPLEV1;
        v = !!(gpio_val & mask);
    }
    return v;
}

void gpio_set_pull(unsigned pin, gpio_pull_t pull) {
    if (GPIO_CHECK_INVALID(pin)) return; 
    volatile unsigned int *gppud = GPPUD0 + (pin / 16); 
    unsigned int offset = (pin % 16) * 2;
    unsigned int pud = (*gppud); 
    pud &= ~(0b11 << offset); 
    pud |= ((pull & 0b11) << offset); 
    (*gppud) = pud; 
    delay_us(125); 
}
