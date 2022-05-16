#include "common.h"
#include "gpio.h"
#include "delay.h"
#include "uart.h"
#include "printf.h"
#include "uartgetcode.h"
int main(void) {
    gpio_set_function(20, GPIO_FUNC_OUTPUT); 
    gpio_set_function(4, GPIO_FUNC_OUTPUT); 
    gpio_set_function(5, GPIO_FUNC_OUTPUT); 
    gpio_set_function(8, GPIO_FUNC_OUTPUT); 
    gpio_set_function(9, GPIO_FUNC_OUTPUT); 
    gpio_set_function(16, GPIO_FUNC_OUTPUT); 

    uart_init(UART0_BASE, UART0_TX, UART0_RX, UART0_PIN_FUNC, UART0_PIN_FUNC, 115200); 
    uart_init(UART5_BASE, UART5_TX, UART5_RX, UART5_PIN_FUNC, UART5_PIN_FUNC, 115200); 

    printf("hello!!! from secondary program!!!\n"); 
    uart_putc(UART0_BASE, '\n');
    uart_putc(UART0_BASE, '\n');

    dev_barrier(); 

    return 0; 
} 


