#include "nes_state.h"
#include "cpu.h"
#include "ppu.h"
#include "bus.h"
#include "controller.h"
struct nes_memory system_memory; 

void nes_memory_init(void) {
    memset((uint8_t*)system_memory.oam_data, 0, 256); 
    memset((uint8_t*)system_memory.palette_table, 0, 32); 
    memset((uint8_t*)system_memory.vram, 0, 2048); 
    memset((uint8_t*)system_memory.cpu_ram, 0, CPU_RAM_SIZE); 
}


struct nes_cpu6502 cpu; 
struct nes_ppu ppu; 
struct nes_bus bus; 
struct nes_controller controller1; 
struct nes_controller controller2; 
struct nes_rom rom; 




