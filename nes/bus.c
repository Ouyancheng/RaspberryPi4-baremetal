#include "bus.h"
#include "ppu.h"
uint8_t cpu_ram[CPU_RAM_SIZE]; 


struct nes_rom rom; 

void bus_init(struct nes_rom bus_rom) {
    memset((uint8_t*)cpu_ram, 0, CPU_RAM_SIZE); 
    rom = bus_rom;
    ppu_init(rom.chr_rom, rom.chr_rom_size, rom.mirroring); 
}
uint8_t bus_read(uint16_t addr) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        return cpu_ram[actual_addr]; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        switch (actual_addr) {
            case 0x2007:
                return ppu_read_data(); 
            case 0x2004: 
                // OAM data 
                return ppu_read_oam_data(); 
            case 0x2002: 
                // status 
                return ppu_read_status(); 
            default: 
                panic("reading invalid ppu register %04x\n", addr); 
                break; 
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
                ppu_write_ctrl(value); 
                break; 
            case 0x2001:
                // mask 
                ppu_write_mask(value); 
                break; 
            case 0x2002: 
                panic("the ppu status register is read-only! addr=%04x\n", addr); 
                break; 
            case 0x2003: 
                // OAM addr 
                ppu_write_oam_addr(value); 
                break; 
            case 0x2004: 
                // OAM data 
                ppu_write_oam_data(value); 
                break; 
            case 0x2005: 
                // scroll 
                ppu_write_scroll(value); 
                break; 
            case 0x2006: 
                // ppu addr 
                ppu_write_addr(value); 
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

void bus_catch_up_cpu_cycles(unsigned cpu_cycles) {
    ppu_tick_cycles(cpu_cycles * 3); 
}
bool bus_poll_nmi(void) {
    if (ppu.nmi_raised) {
        ppu.nmi_raised = false; 
        return true; 
    } else {
        return false; 
    }
}







