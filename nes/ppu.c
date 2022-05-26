#include "ppu.h"

struct ppu_device ppu; 

void ppu_init(uint8_t *chr_rom, size_t chr_rom_size, enum rom_mirror mirror) {
    ppu.chr_rom = chr_rom; 
    ppu.chr_rom_size = chr_rom_size; 
    ppu.mirror = mirror; 
    memset((uint8_t*)ppu.oam_data, 0, 256); 
    memset((uint8_t*)ppu.palette_table, 0, 32); 
    memset((uint8_t*)ppu.vram, 0, 2048); 
    ppu.addr.value = 0; 
    ppu.addr.reading_lo = 0; 
    ppu.scroll.x = 0; 
    ppu.scroll.y = 0; 
    ppu.scroll.reading_y = 0; 
    ppu.ctrl = 0; 
    ppu.mask = 0; 
    ppu.status = 0; 
    ppu.buffer = 0; 
}
void ppu_addr_write(uint8_t data) {
    if (ppu.addr.reading_lo) {
        ppu.addr.value &= 0xFF00; 
        ppu.addr.value |= (uint16_t)data; 
    } else {
        ppu.addr.value &= 0x00FF; 
        ppu.addr.value |= (((uint16_t)data) << 8); 
    }
    ppu.addr.value &= 0x3fff; 
    ppu.addr.reading_lo = !ppu.addr.reading_lo; 
}
static inline void ppu_addr_increment(uint8_t inc) {
    ppu.addr.value += (uint16_t)inc; 
    ppu.addr.value &= 0x3fff; 
}
void ppu_addr_reset_state(void) {
    ppu.addr.reading_lo = 0; 
}
static inline void ppu_vram_addr_increment(void) {
    ppu_addr_increment((ppu.ctrl & ppu_ctrl_vram_addr_increment) ? 32 : 1); 
}
uint16_t ppu_mirror_vram_addr(uint16_t addr) {
    unsigned index = (addr & 0b10111111111111) - 0x2000; // 0x2FFF
    unsigned quadrant = index / 0x400; // 2000, 2400, 2800, 2C00  
    switch (ppu.mirror) {
        case rom_mirror_vertical:
            return (quadrant >= 2) ? (index - 0x800) : (index); 
        case rom_mirror_horizontal: 
            if (quadrant == 2 || quadrant == 1) return index - 0x400; 
            else if (quadrant == 3) return index - 0x800; 
            else return index; 
        case rom_mirror_four_screen: 
            return index - 0x400 * quadrant;
        default: 
            return index; 
    }
}
uint8_t ppu_read_data(void) {
    uint16_t addr = ppu.addr.value; 
    ppu_vram_addr_increment(); 
    uint8_t result = ppu.buffer; 
    if (addr < 0x2000) {
        result = ppu.buffer; 
        ppu.buffer = ppu.chr_rom[addr]; 
    } else if (0x2000 <= addr && addr < 0x3000) {
        result = ppu.buffer; 
        ppu.buffer = ppu.vram[ppu_mirror_vram_addr(addr)]; 
    } else if (0x3000 <= addr && addr < 0x3f00) {
        panic("unknown ppu address %04x\n", addr); 
    } else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c) {
        result = ppu.palette_table[((addr-0x10) - 0x3f00)]; 
    } else {
        result = ppu.palette_table[addr - 0x3f00]; 
    }
    return result; 
}
void ppu_write_data(uint8_t data) {
    uint16_t addr = ppu.addr.value; 
    ppu_vram_addr_increment(); 
    if (addr < 0x2000) {
        panic("attempting to write to chr rom address %04x\n", addr); 
    } else if (0x2000 <= addr && addr < 0x3000) {
        ppu.vram[ppu_mirror_vram_addr(addr)] = data; 
    } else if (0x3000 <= addr && addr < 0x3f00) {
        panic("unknown ppu address %04x\n", addr); 
    } else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c) {
        ppu.palette_table[((addr-0x10) - 0x3f00)] = data; 
    } else {
        ppu.palette_table[addr - 0x3f00] = data; 
    }
}
struct rgb ppu_mask_get_color_emphasis(void) {
    struct rgb result; 
    result.r = !!(ppu.mask & ppu_mask_emphasize_red);
    result.g = !!(ppu.mask & ppu_mask_emphasize_green);
    result.b = !!(ppu.mask & ppu_mask_emphasize_blue);
    return result; 
}
void ppu_scroll_write(uint8_t value) {
    if (ppu.scroll.reading_y) {
        ppu.scroll.y = value; 
    } else {
        ppu.scroll.x = value; 
    }
    ppu.scroll.reading_y = !ppu.scroll.reading_y; 
}
void ppu_scroll_reset_state(void) {
    ppu.scroll.reading_y = 0; 
}

