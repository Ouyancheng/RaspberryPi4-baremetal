#pragma once 

#include "mbox.h"
#include "gpio.h"
#include "delay.h"
#include "boot.h"
#include <stdint.h>
#define UART_CLK (4000000) 
/** 
 * Sets the UART clock to 4MHz. 
 * The clock is only set once, any subsequent calls to uart_set_clock will have no effect 
 */
void uart_set_clock(); 
/**
 * The PL011 UART controller, this only handles basic read and write. 
 * Some good resources: 
 * https://raspberrypi.stackexchange.com/questions/45570/how-do-i-make-serial-work-on-the-raspberry-pi3-pizerow-pi4-or-later-models/107780#107780 
 * https://github.com/ethanfaust/rpi4-baremetal-uart 
 * https://www.raspberrypi.com/documentation/computers/configuration.html#configuring-uarts 
 */
template <
    uint32_t baseaddr, 
    uint32_t txpin, gpio_func_t txpinfunc, 
    uint32_t rxpin, gpio_func_t rxpinfunc
>
class PL011UART {
    using io32p = volatile uint32_t *; 
    /// data register 
    static constexpr uintptr_t DR = (baseaddr+(0x00)); 
    /// ? 
    static constexpr uintptr_t RSRECR = baseaddr+(0x04); 
    /// flag register 
    static constexpr uintptr_t FR = baseaddr+(0x18); 
    /// not in use
    static constexpr uintptr_t ILPR = baseaddr+(0x20); 
    /// Integer Baud rate divisor
    static constexpr uintptr_t IBRD = baseaddr+(0x24); 
    /// Fractional Baud rate divisor
    static constexpr uintptr_t FBRD = baseaddr+(0x28); 
    /// Line Control register
    static constexpr uintptr_t LCRH = baseaddr+(0x2c); 
    /// Control register
    static constexpr uintptr_t CR = baseaddr+(0x30); 
    /// Interrupt FIFO Level Select Register
    static constexpr uintptr_t IFLS = baseaddr+(0x34); 
    /// Interrupt Mask Set Clear Register
    static constexpr uintptr_t IMSC = baseaddr+(0x38); 
    /// Raw Interrupt Status Register
    static constexpr uintptr_t RIS = baseaddr+(0x3c); 
    /// Masked Interrupt Status Register
    static constexpr uintptr_t MIS = baseaddr+(0x40); 
    /// Interrupt Clear Register
    static constexpr uintptr_t ICR = baseaddr+(0x44); 
    /// DMA Control Register
    static constexpr uintptr_t DMACR = baseaddr+(0x48); 
    /// Test Control register
    static constexpr uintptr_t ITCR = baseaddr+(0x80); 
    /// Integration test input reg 
    static constexpr uintptr_t ITIP = baseaddr+(0x84); 
    /// Integration test output reg 
    static constexpr uintptr_t ITOP = baseaddr+(0x88); 
    /// Test Data reg 
    static constexpr uintptr_t TDR = baseaddr+(0x8c); 
    uint32_t current_baud; 
public: 
    PL011UART() : current_baud(115200) {}
    void init(uint32_t baud = 115200) {
        // turn off tx and rx 
        put32(CR, 0); 
        
        uart_set_clock(); 

        gpio_set_function(txpin, txpinfunc); 
        gpio_set_function(rxpin, rxpinfunc); 
        dev_barrier(); 
        delay_ms(1); 

        gpio_set_pull(txpin, GPIO_PULL_NONE); 
        gpio_set_pull(rxpin, GPIO_PULL_NONE); 
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
        put32(ICR, 0x7FF); 
        put32(IBRD, divider); 
        put32(FBRD, fraction); 
        put32(LCRH, 0b11 << 5); 
        put32(CR, 0x301); 
        current_baud = baud; 
        dev_barrier(); 
    }

    void send(const char c) {
        do {
            asm volatile("nop");
        } while(get32(FR) & 0x20);
        // *DR = c;
        put32(DR, (c & 0xff)); 
        dev_barrier(); 
    }
    uint32_t getc() {
        do {
            asm volatile("nop");
        } while (get32(FR) & 0x10);
        return get32(DR) & 0xff;
    }
    void puts(const char *s) {
        while (*s) {
            send(*s);
            ++s; 
        }
    }
    constexpr uint32_t get_txpin() {
        return txpin;
    }
    constexpr uint32_t get_rxpin() {
        return rxpin;
    }
    uint32_t get_baud() {
        return current_baud; 
    }
    void disable() {
        put32(CR, 0); 
        dev_barrier(); 
    }
};

extern PL011UART<0xfe201a00, 12, GPIO_FUNC_ALT4, 13, GPIO_FUNC_ALT4> uart5; 
extern PL011UART<0xfe201000, 14, GPIO_FUNC_ALT0, 15, GPIO_FUNC_ALT0> uart0; 
extern PL011UART<0xfe201400, 0,  GPIO_FUNC_ALT4, 1,  GPIO_FUNC_ALT4> uart2;
extern PL011UART<0xfe201600, 4,  GPIO_FUNC_ALT4, 5,  GPIO_FUNC_ALT4> uart3;
extern PL011UART<0xfe201800, 8,  GPIO_FUNC_ALT4, 9,  GPIO_FUNC_ALT4> uart4; 


