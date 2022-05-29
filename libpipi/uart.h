#ifndef UART_H 
#define UART_H 

#include "mbox.h"
#include "gpio.h"
#include "delay.h"
#include "common.h"

#define UART_CLK (4000000) 

/** 
 * Sets the UART clock to 4MHz. 
 * The clock is only set once, any subsequent calls to uart_set_clock will have no effect 
 */
void uart_set_clock(); 

/// data register 
#define DR(baseaddr) ((baseaddr)+(0x00))  
/// ? 
#define RSRECR(baseaddr) ((baseaddr)+(0x04))  
/// flag register 
#define FR(baseaddr) ((baseaddr)+(0x18))  
/// not in use
#define ILPR(baseaddr) ((baseaddr)+(0x20)) 
/// Integer Baud rate divisor
#define IBRD(baseaddr) ((baseaddr)+(0x24)) 
/// Fractional Baud rate divisor
#define FBRD(baseaddr) ((baseaddr)+(0x28)) 
/// Line Control register
#define LCRH(baseaddr) ((baseaddr)+(0x2c)) 
/// Control register
#define CR(baseaddr) ((baseaddr)+(0x30)) 
/// Interrupt FIFO Level Select Register
#define IFLS(baseaddr) ((baseaddr)+(0x34)) 
/// Interrupt Mask Set Clear Register
#define IMSC(baseaddr) ((baseaddr)+(0x38)) 
/// Raw Interrupt Status Register
#define RIS(baseaddr) ((baseaddr)+(0x3c)) 
/// Masked Interrupt Status Register
#define MIS(baseaddr) ((baseaddr)+(0x40)) 
/// Interrupt Clear Register
#define ICR(baseaddr) ((baseaddr)+(0x44)) 
/// DMA Control Register
#define MACR(baseaddr) ((baseaddr)+(0x48)) 
/// Test Control register
#define ITCR(baseaddr) ((baseaddr)+(0x80)) 
/// Integration test input reg 
#define ITIP(baseaddr) ((baseaddr)+(0x84)) 
/// Integration test output reg 
#define ITOP(baseaddr) ((baseaddr)+(0x88)) 
/// Test Data reg 
#define TDR(baseaddr) ((baseaddr)+(0x8c)) 

#define UART0_BASE (MMIO_BASE+0x00201000)
#define UART2_BASE (MMIO_BASE+0x00201400)
#define UART3_BASE (MMIO_BASE+0x00201600)
#define UART4_BASE (MMIO_BASE+0x00201800)
#define UART5_BASE (MMIO_BASE+0x00201a00)

#define UART0_TX (14)
#define UART0_RX (15)
#define UART2_TX (0)
#define UART2_RX (1)
#define UART3_TX (4)
#define UART3_RX (5)
#define UART4_TX (8)
#define UART4_RX (9)
#define UART5_TX (12)
#define UART5_RX (13) 

#define UART0_PIN_FUNC (GPIO_FUNC_ALT0) 
#define UART2_PIN_FUNC (GPIO_FUNC_ALT4) 
#define UART3_PIN_FUNC (GPIO_FUNC_ALT4) 
#define UART4_PIN_FUNC (GPIO_FUNC_ALT4) 
#define UART5_PIN_FUNC (GPIO_FUNC_ALT4) 



void uart_init(uintptr_t uart_base_addr, unsigned txpin, unsigned rxpin, gpio_func_t txpinfunc, gpio_func_t rxpinfunc, uint32_t baud);
void uart_putc(uintptr_t baseaddr, const char c);
char uart_getc(uintptr_t baseaddr);
int uart_has_data(uintptr_t baseaddr); 
void uart_puts(uintptr_t baseaddr, const char *s);
void uart_disable(uintptr_t baseaddr);




#endif 
