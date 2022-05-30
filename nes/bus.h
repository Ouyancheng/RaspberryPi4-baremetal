#pragma once 
#include "sdk.h"
#define ADDR_MAX 0xFFFF 


struct nes_bus {
    uint8_t *cpu_ram; 
    uint8_t *prg_rom; 
    unsigned cycles; 
};
extern struct nes_bus bus; 

void bus_init(void); 
uint8_t bus_read(uint16_t addr); 
void bus_write(uint16_t addr, uint8_t value); 
uint8_t read_prg_rom(uint16_t addr); 


void bus_catch_up_cpu_cycles(unsigned cpu_cycles); 

bool bus_poll_nmi(void); 

