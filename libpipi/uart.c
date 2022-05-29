#include "uart.h"

void uart_set_clock() {
    static int done = 0;
    if (!done) {
        mbox[0] = 9*4;
        mbox[1] = MBOX_REQUEST;
        mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
        mbox[3] = 12;
        mbox[4] = 8;
        mbox[5] = 2;             // UART clock
        mbox[6] = UART_CLK;      // 4 MHz
        mbox[7] = 0;             // clear turbo
        mbox[8] = MBOX_TAG_LAST;
        mbox_call(MBOX_CH_PROP);
        done = 1; 
    }
}

void uart_init(uintptr_t uart_base_addr, unsigned txpin, unsigned rxpin, gpio_func_t txpinfunc, gpio_func_t rxpinfunc, uint32_t baud) {
    // turn off tx and rx 
    put32(CR(uart_base_addr), 0); 
    uart_set_clock(); 
    gpio_set_function(txpin, txpinfunc); 
    gpio_set_function(rxpin, rxpinfunc); 
    dev_barrier(); 
    delay_ms(1); 
    // gpio_set_pull(txpin, GPIO_PULL_NONE); 
    // gpio_set_pull(rxpin, GPIO_PULL_NONE); 
    dev_barrier(); 
    delay_ms(1);
    // *ICR = 0x7FF; 
    // *IBRD = 2; 
    // *FBRD = 0xB; 
    // *LCRH = 0b11 << 5; 
    // *CR = 0x301; 
    /**
        divider = UART_CLK / (16 * baud)
        temp = (((UART_CLK % (16 * baud)) * 8) / baud)
        fraction = (temp >> 1) + (temp & 1)
    */
    uint32_t divider = UART_CLK / (16 * baud); 
    uint32_t temp = (((UART_CLK % (16 * baud)) * 8) / baud); 
    uint32_t fraction = (temp >> 1) + (temp & 1); 
    put32(ICR(uart_base_addr), 0x7FF); 
    put32(IBRD(uart_base_addr), divider); 
    put32(FBRD(uart_base_addr), fraction); 
    put32(LCRH(uart_base_addr), 0b11 << 5); 
    put32(CR(uart_base_addr), 0x301); 
    dev_barrier(); 
}
void uart_putc(uintptr_t baseaddr, const char c) {
    do {
        asm volatile("nop");
    } while(get32(FR(baseaddr)) & 0x20);
    put32(DR(baseaddr), (c & 0xff)); 
    dev_barrier(); 
}
char uart_getc(uintptr_t baseaddr) {
    do {
        asm volatile("nop");
    } while (get32(FR(baseaddr)) & 0x10);
    return get32(DR(baseaddr)) & 0xff;
}
int uart_has_data(uintptr_t baseaddr) {
    return !(get32(FR(baseaddr)) & 0x10); 
}
void uart_puts(uintptr_t baseaddr, const char *s) {
    while (*s) {
        uart_putc(baseaddr, *s);
        ++s; 
    }
}
void uart_disable(uintptr_t baseaddr) {
    put32(CR(baseaddr), 0); 
    dev_barrier(); 
}


