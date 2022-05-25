#include "bus.h"

uint8_t cpu_ram[CPU_RAM_SIZE]; 


struct nes_rom rom; 

void bus_init(struct nes_rom bus_rom) {
    memset((uint8_t*)cpu_ram, 0, CPU_RAM_SIZE); 
    rom = bus_rom;
}
uint8_t bus_read(uint16_t addr) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        return cpu_ram[actual_addr]; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        printf("ppu address %04x\n", actual_addr); 
        return 0; 
    } 
    else if (0x8000 <= addr) {
        return read_prg_rom(addr); 
    }
    else {
        printf("reading from address %04x is ignored\n", addr); 
        return 0; 
    }
}

void bus_write(uint16_t addr, uint8_t value) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        cpu_ram[actual_addr] = value; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        printf("ppu address %04x\n", actual_addr); 
    } 
    else if (0x8000 <= addr) {
        panic("Error: trying to write to PRG ROM space!\n"); 
    }
    else {
        printf("writing to address %04x is ignored\n", addr); 
    }
}

uint8_t read_prg_rom(uint16_t addr) {
    addr -= UINT16_C(0x8000);
    if ((rom.prg_rom_size == 0x4000) && (addr >= 0x4000)) {
        addr %= 0x4000; 
    }
    return rom.prg_rom[addr]; 
}

