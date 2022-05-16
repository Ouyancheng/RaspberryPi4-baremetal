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
    // int cnt = 0; 
    // while (cnt < 10) {
    //     uint32_t c = uart_getc(UART0_BASE); 
    //     uart_putc(UART5_BASE, (const char)c); 
    //     uart_puts(UART0_BASE, "Hello World!!! "); 
    //     uart_putc(UART0_BASE, (const char)c); 
    //     uart_putc(UART0_BASE, ' '); 
    //     cnt += 1; 
    // }

    // while (1) {
    //     printf("hello!\n"); 
    //     delay_ms(1000); 
    // }

    uintptr_t addr = uart_get_code(0x180000); 
    if (!addr) {
        printf("boot failed!\n"); 
        rpi_reboot(); 
    }

    void(*start_function)(void) = (void(*)(void))(addr); 
    start_function(); 

    return 0; 
} 


