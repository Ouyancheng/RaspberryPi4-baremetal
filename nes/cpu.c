#include "cpu.h"
#include "cpu_exec.h"



struct cpu_interrupt_info irq_info[3] = {
    {.type = cpu_interrupt_nmi,     .flag_mask = (uint8_t)flag_break2, .cycles = 2},
    {.type = cpu_interrupt_reset,   .flag_mask = (uint8_t)flag_break2, .cycles = 2}, // TODO 
    {.type = cpu_interrupt_irq_brk, .flag_mask = (uint8_t)flag_break2, .cycles = 2}  // TODO 
}; 

void cpu_init(void) {
    cpu.a = 0; 
    cpu.x = 0; 
    cpu.y = 0; 
    cpu.p = 0b00100100; 
    cpu.pc = 0;
    cpu.sp = SP_RESET; 
    cpu.page_crossed = false; 
    cpu.delayed_cycles = 0; 
}

void cpu_load_program(uint8_t *program, size_t length, unsigned at) {
    for (size_t i = 0; i < length; ++i) {
        cpu_mem_write(UINT16_C(0x0600) + (uint16_t)i, program[i]); 
    }
}
////// NOTE: this is the reset interrupt 
void cpu_reset(void) {
    cpu.a = 0; 
    cpu.x = 0; 
    cpu.y = 0; 
    cpu.sp = SP_RESET; // this is 0xFD because we pretend that we have pushed something here... 
    cpu.p = (uint8_t)flag_break2 | (uint8_t)flag_interrupt_disable; // UINT8_C(0b00100100); 
    cpu.pc = cpu_mem_read16(INTERRUPT_VECTOR_BASE + (uint16_t)cpu_interrupt_reset * UINT16_C(2)); 
}
void cpu_run(void) {
    cpu_run_with_callback(NULL); 
}

void cpu_load_program_and_run(uint8_t *program, size_t length, unsigned at) {
    cpu_load_program(program, length, at); 
    cpu_reset(); 
    cpu_run(); 
}



