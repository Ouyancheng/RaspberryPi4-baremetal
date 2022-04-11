#include "uart.hpp"

void uart_set_clock() {
    static int done = 0;
    if (!done) {
        mbox[0] = 9*4;
        mbox[1] = MBOX_REQUEST;
        mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
        mbox[3] = 12;
        mbox[4] = 8;
        mbox[5] = 2;             // UART clock
        mbox[6] = UART_CLK;       // 4 MHz
        mbox[7] = 0;             // clear turbo
        mbox[8] = MBOX_TAG_LAST;
        mbox_call(MBOX_CH_PROP);
        done = 1; 
    }
}


PL011UART<0xfe201a00, 12, GPIO_FUNC_ALT4, 13, GPIO_FUNC_ALT4> uart5; 
PL011UART<0xfe201000, 14, GPIO_FUNC_ALT0, 15, GPIO_FUNC_ALT0> uart0; 
PL011UART<0xfe201400, 0,  GPIO_FUNC_ALT4, 1,  GPIO_FUNC_ALT4> uart2;
PL011UART<0xfe201600, 4,  GPIO_FUNC_ALT4, 5,  GPIO_FUNC_ALT4> uart3;
PL011UART<0xfe201800, 8,  GPIO_FUNC_ALT4, 9,  GPIO_FUNC_ALT4> uart4; 





