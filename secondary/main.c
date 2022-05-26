#include "common.h"
#include "gpio.h"
#include "delay.h"
#include "uart.h"
#include "printf.h"
#include "framebuffer.h"
#include "c_string.h"
uint32_t scratch_buffer[256][240]; 
void int_to_dec_str(char *buf, unsigned n) {
    char buffer[12]; 
    int i = 0;
    if (n == 0) {
        buf[0] = '0'; 
        buf[1] = 0; 
        return; 
    }
    while (n) {
        buffer[i++] = '0' + (n % 10); 
        n /= 10; 
    }
    int j = 0; 
    while (i --> 0) {
        buf[j++] = buffer[i]; 
    }
    buf[j] = 0; 
}
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

    dev_barrier(); 
    // nes standard 250x240 pixels 
    framebuffer_init(256, 240, 32, 0); 
    printf("width=%d, height=%d\n", width, height); 
    
    printf("press any key to continue...\n"); 
    dev_barrier(); 
    int t = 0; 
    int time_used = 0;
    int cnt = 0; 
    
    while (!uart_has_data(UART0_BASE)) {
        unsigned now = get_current_time_us(); 
        for (int i = 10; i < width; i += 1) {
            for (int j = 10; j < height; j += 1) {
                // draw_pixel_rgba((unsigned char*)fb, i, j, (t+i+j)&0xff, (t+i)&0xff, (t+j)&0xff, 0x0); 
                draw_pixel_rgba((unsigned char*)fb, i, j, (t)&0xff, (t*2)&0xff, (t*4)&0xff, 0x0); 
                // draw_pixel(i, j, (0x0e + t + i + j)&0xff); 
            }
        }
        // memcpy(fb, scratch_buffer, width*height*4); 
        char buf[12]; 
        memset(buf, 0, 12); 
        // sprintf(buf, "%d", time_used); 
        int_to_dec_str(buf, cnt); 
        draw_string(2, 2, buf, 0x0f); 
        
        t += 1; 
        cnt = 0; 
        time_used = get_current_time_us()-now; 
        // 1000000/60 = 16666
        while (get_current_time_us() - now < 16666) {
            cnt += 1; 
            // if (cnt == 10000) {
            //     framebuffer_display_and_swap(); 
            // }
        }
        // delay_us(16666); 
    }
    return 0; 
} 


