#include "common.h"
#include "gpio.h"
#include "delay.h"
#include "uart.h"
#include "printf.h"
#include "framebuffer.h"
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

    framebuffer_init(1920, 1080, 1920, 1080, 0, 0, 32); 

    draw_rectangle(150,150,400,400,0x03,0);
    draw_rectangle(300,300,350,350,0x2e,1);

    draw_circle(960,540,250,0x0e,0);
    draw_circle(960,540,50,0x13,1);

    draw_pixel(250,250,0x0e);

    draw_char('O',500,500,0x05);
    draw_string(100,100,"Hello world!",0x0f);

    draw_line(100,500,350,700,0x0c);

    delay_sec(30);

    return 0; 
} 


