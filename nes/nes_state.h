#pragma once 
#include "sdk.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"
#include "rom.h"
#include "controller.h"
#define CPU_RAM_SIZE 2048 
#define ROM_SIZE_MAX (1024*1024)
struct nes_memory {
    /// RAM 
    uint8_t cpu_ram[CPU_RAM_SIZE]; 
    /// PPU 
    uint8_t palette_table[32]; 
    uint8_t vram[2048]; 
    uint8_t oam_data[256]; 
    /// ROM 
    uint8_t rom_binary[ROM_SIZE_MAX]; 
};
extern struct nes_memory system_memory; 
extern struct nes_cpu6502 cpu; 
extern struct nes_ppu ppu; 
extern struct nes_bus bus; 
extern struct nes_controller controller1; 
extern struct nes_controller controller2; 
extern struct nes_rom rom; 

void nes_memory_init(void); 










