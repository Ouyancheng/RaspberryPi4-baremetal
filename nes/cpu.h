#pragma once 
#include "sdk.h"
#include "cpu_opcodes.h"
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
struct cpustate {
    uint8_t  a; 
    uint8_t  x; 
    uint8_t  y; 
    uint8_t  p;  // status 
    uint16_t pc; 
    uint8_t  sp; 
};

#define ADDR_MAX 0xFFFF 

extern uint8_t nes_memory[ADDR_MAX+1]; 
extern struct cpustate cpu; 

static inline uint8_t cpu_mem_read(uint16_t addr) {
    return nes_memory[addr]; 
}
static inline void cpu_mem_write(uint16_t addr, uint8_t value) {
    nes_memory[addr] = value; 
}

static inline uint16_t cpu_mem_read16(uint16_t addr) {
    return (((uint16_t)cpu_mem_read(addr+1)) << 8) | ((uint16_t)cpu_mem_read(addr)); 
}
static inline void cpu_mem_write16(uint16_t addr, uint16_t value) {
    cpu_mem_write(addr, value & 0xFF); 
    cpu_mem_write(addr+1, value >> 8); 
}
void cpu_load_program_and_run(uint8_t *program, size_t length); 
void cpu_run(void); 
void cpu_run_with_callback(void(*func)(void)); 
void cpu_reset(void); 
void cpu_load_program(uint8_t *program, size_t length); 
void cpu_init(void); 



