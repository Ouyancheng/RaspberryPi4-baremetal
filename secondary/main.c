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
    framebuffer_init(256, 240, 32); 
    printf("width=%d, height=%d\n", width, height); 

    printf("press any key to continue...\n"); 
    dev_barrier(); 
    int t = 0; 
    // printf("outer function: fb = %p, fb1=%p, fb2=%p\n", fb, fb1, fb2); 
    while (!uart_has_data(UART0_BASE)) {
        // printf("outer function1: fb = %p, fb1=%p, fb2=%p\n", fb, fb1, fb2); 
        unsigned now = get_current_time_us(); 
        for (int i = 0; i < width; i += 1) {
            for (int j = 0; j < height; j += 1) {
                draw_pixel_rgba((unsigned char*)scratch_buffer, i, j, (t+i+j)&0xff, (t+i)&0xff, (t+j)&0xff, 0x0); 
            }
        }
        // printf("outer function2: fb = %p, fb1=%p, fb2=%p\n", fb, fb1, fb2); 
        // memset(fb, (t)&0xff, width*height*sizeof(uint32_t)); 
        memcpy(fb, scratch_buffer, width*height*4); 
        // printf("outer function3: fb = %p, fb1=%p, fb2=%p\n", fb, fb1, fb2); 
        // int ret; 
        // if ((ret=framebuffer_display_and_swap()) != 0) {
        //     printf("framebuffer swap failed! ret=%d \n", ret); 
        //     printf("123456\n");
        //     return 1; 
        // } 
        t += 1; 
        int cnt = 0; 
        // 1000000/60 = 16666
        while (get_current_time_us() - now < 16666) {
            cnt += 1; 
        }
        // printf("cnt=%d diff=%d\n", cnt, get_current_time_us()-now); 
        // delay_sec(1);
    }
    return 0; 
} 


