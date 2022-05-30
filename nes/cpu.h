#pragma once 
#include "sdk.h"
#include "bus.h"
#include "cpu_opcodes.h"
#include "cpu_exec.h"
enum cpuflags {
    flag_carry             = UINT8_C(0b00000001),
    flag_zero              = UINT8_C(0b00000010),
    flag_interrupt_disable = UINT8_C(0b00000100),
    flag_decimal_mode      = UINT8_C(0b00001000), // not supported 
    flag_break             = UINT8_C(0b00010000),
    flag_break2            = UINT8_C(0b00100000),
    flag_overflow          = UINT8_C(0b01000000), 
    flag_negative          = UINT8_C(0b10000000)
};

#define STACK_OFFSET UINT16_C(0x0100) 
#define SP_RESET 0xFD 
struct nes_cpu6502 {
    uint8_t  a; 
    uint8_t  x; 
    uint8_t  y; 
    uint8_t  p;  // status 
    uint16_t pc; 
    uint8_t  sp; 
    // the catchup mechanism 
    bool page_crossed; 
    unsigned delayed_cycles; 
};

extern struct nes_cpu6502 cpu; 

static inline uint8_t cpu_mem_read(uint16_t addr) {
    // return nes_memory[addr]; 
    return bus_read(addr); 
}
static inline void cpu_mem_write(uint16_t addr, uint8_t value) {
    // nes_memory[addr] = value; 
    bus_write(addr, value); 
}

static inline uint16_t cpu_mem_read16(uint16_t addr) {
    return (((uint16_t)cpu_mem_read(addr+1)) << 8) | ((uint16_t)cpu_mem_read(addr)); 
}
static inline void cpu_mem_write16(uint16_t addr, uint16_t value) {
    cpu_mem_write(addr, value & 0xFF); 
    cpu_mem_write(addr+1, value >> 8); 
}
////// NOTE: The following two function are only for test purpose!!!!!! 
void cpu_load_program(uint8_t *program, size_t length, unsigned at); 
void cpu_load_program_and_run(uint8_t *program, size_t length, unsigned at); 
////// NOTE: Run the CPU, notice that the rom should be load and the bus should be initialized 
void cpu_run(void); 
void cpu_run_with_callback(void(*func)(void)); 
////// The reset interrupt 
void cpu_reset(void); 
////// initialization 
void cpu_init(void); 
uint16_t cpu_get_address(enum addressmode mode, uint16_t addr); 
enum cpu_interrupt {
    cpu_interrupt_nmi     = UINT16_C(0),
    cpu_interrupt_reset   = UINT16_C(1), 
    cpu_interrupt_irq_brk = UINT16_C(2) 
};
#define INTERRUPT_VECTOR_BASE (UINT16_C(0xFFFA))
struct cpu_interrupt_info {
    enum cpu_interrupt type; // handler address = interrupt vector base + type * 2 
    uint8_t flag_mask; 
    unsigned cycles; 
};
extern struct cpu_interrupt_info irq_info[3];
