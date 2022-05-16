#include "common.h"
#include "gpio.h"
#include "delay.h"
#include "uart.h"
#include "printf.h"
#include "framebuffer.h"
#include "c_string.h"
uint32_t scratch_buffer[256][240]; 
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
    // uart_putc(UART0_BASE, '\n');
    // uart_putc(UART0_BASE, '\n');

    dev_barrier(); 
    // nes standard 250x240 pixels 
    framebuffer_init(256, 240, 256, 240, 0, 0, 32); 
    printf("width=%d, height=%d\n", width, height); 

    printf("press any key to continue...\n"); 
    dev_barrier(); 
    int t = 0; 
    while (!uart_has_data(UART0_BASE)) {
        unsigned now = get_current_time_us(); 
        for (int i = 0; i < width; i += 1) {
            for (int j = 0; j < height; j += 1) {
                draw_pixel_rgba((unsigned char*)scratch_buffer, i, j, (t)&0xff, (t)&0xff, 0x0, 0x0); 
            }
        }
        // memset(fb, (t)&0xff, width*height*sizeof(uint32_t)); 
        memcpy(fb, scratch_buffer, width*height*4); 
        t += 1; 
        while (get_current_time_us() - now < 16000); 
    }
    return 0; 
} 


