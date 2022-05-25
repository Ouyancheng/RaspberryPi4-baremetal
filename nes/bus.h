#pragma once 
#include "sdk.h"
#include "rom.h"
#define ADDR_MAX 0xFFFF 
#define CPU_RAM_SIZE 2048 
extern uint8_t cpu_ram[CPU_RAM_SIZE]; 

void bus_init(struct nes_rom bus_rom); 
uint8_t bus_read(uint16_t addr); 
void bus_write(uint16_t addr, uint8_t value); 
uint8_t read_prg_rom(uint16_t addr); 
extern struct nes_rom rom; 
