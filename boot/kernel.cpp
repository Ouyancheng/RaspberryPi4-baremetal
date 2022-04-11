#include "boot.h"
#include "gpio.h"
#include "delay.h"
#include "uart.hpp"
#include <stdint.h>

int main(void) {
    // gpio_set_pull(14, GPIO_PULL_UP); 
    gpio_set_function(20, GPIO_FUNC_OUTPUT); 
    gpio_set_function(4, GPIO_FUNC_OUTPUT); 
    gpio_set_function(5, GPIO_FUNC_OUTPUT); 
    gpio_set_function(8, GPIO_FUNC_OUTPUT); 
    gpio_set_function(9, GPIO_FUNC_OUTPUT); 
    // gpio_set_function(12, GPIO_FUNC_OUTPUT); 
    // gpio_set_function(13, GPIO_FUNC_OUTPUT); 
    // gpio_set_function(14, GPIO_FUNC_OUTPUT); 
    // gpio_set_function(15, GPIO_FUNC_OUTPUT); 
    gpio_set_function(16, GPIO_FUNC_OUTPUT); 

    uart0.init(115200); 
    uart5.init(115200); 

    gpio_set_off(20); 
    while (1) {
        // delay_ms(500); 
        // gpio_set_on(20); 
        // gpio_set_on(4); 
        // gpio_set_on(5); 
        // gpio_set_on(8); 
        // gpio_set_on(9); 
        // // gpio_set_on(12); 
        // // gpio_set_on(13); 
        // // gpio_set_on(14); 
        // // gpio_set_on(15); 
        // gpio_set_on(16); 
        // delay_ms(500); 
        // gpio_set_off(20);
        // gpio_set_off(4); 
        // gpio_set_off(5); 
        // gpio_set_off(8); 
        // gpio_set_off(9);  
        // // gpio_set_off(12); 
        // // gpio_set_off(13); 
        // // gpio_set_off(14); 
        // // gpio_set_off(15); 
        // gpio_set_off(16); 
        uint32_t c = uart0.getc(); 
        uart5.send((const char)c); 
        uart0.puts("Hello World!!! "); 
        uart0.send((const char)c); 
        uart0.send(' '); 
    }
    return 0; 
} 


