#include "bus.h"
#include "ppu.h"
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
        if (actual_addr == 0x2007) {
            return ppu_read_data(); 
        } else if (actual_addr == 0x2004) {
            // OAM data 
        } else if (actual_addr == 0x2002) {
            // status 
        } else {
            panic("reading invalid ppu register %04x\n", addr); 
        }
        return 0xFF; 
    } 
    else if (0x8000 <= addr) {
        return read_prg_rom(addr); 
    }
    else {
        // printf("reading from address %04x is ignored\n", addr); 
        return 0xFF; 
    }
}

void bus_write(uint16_t addr, uint8_t value) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        cpu_ram[actual_addr] = value; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        switch (actual_addr) {
            case 0x2000:
                // ctrl 
                break; 
            case 0x2001:
                // mask 
                break; 
            case 0x2002: 
                panic("the ppu status register is read-only! addr=%04x\n", addr); 
                break; 
            case 0x2003: 
                // OAM addr 
                break; 
            case 0x2004: 
                // OAM data 
                break; 
            case 0x2005: 
                // scroll 
                break; 
            case 0x2006: 
                // ppu addr 
                ppu_addr_write(value); 
                break; 
            case 0x2007: 
                // ppu data 
                ppu_write_data(value); 
                break; 
            default: 
                panic("writing invalid ppu register %04x\n", addr); 
                break; 
        }
    } 
    else if (0x8000 <= addr) {
        panic("Error: trying to write to PRG ROM space!\n"); 
    }
    else {
        // printf("writing to address %04x is ignored\n", addr); 
    }
}

uint8_t read_prg_rom(uint16_t addr) {
    addr -= UINT16_C(0x8000);
    if ((rom.prg_rom_size == 0x4000) && (addr >= 0x4000)) {
        addr %= 0x4000; 
    }
    return rom.prg_rom[addr]; 
}

