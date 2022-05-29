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
    ppu.oam_addr = 0; 
    ppu.buffer = 0; 
    ppu.scanline_cycles = 0; 
    ppu.scanline = 0; 
    ppu.nmi_raised = 0; 
}
void ppu_write_addr(uint8_t data) {
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
void ppu_reset_addr_state(void) {
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
            switch (quadrant) {
                case 1: 
                case 2: 
                    return index - 0x400; 
                case 3: 
                    return index - 0x800; 
                default: 
                    return index; 
            }
            break; 
        /** 
         * NOTE: the four screen mirroring is not mirroring everything to quadrant 0!!! 
         * 4-Screen
         * With additional RAM and/or PPU address mapping present on the cartridge, 
         * 4 unique nametables can be addressed through the PPU bus, 
         * creating a 64x60 tilemap, 
         * allowing for more flexible screen layouts. 
         * Very few games made use of this type of mirroring.
         */
        default: 
            return index; 
    }
}
/**
Address range	Size	Description
$0000-$0FFF	$1000	Pattern table 0
$1000-$1FFF	$1000	Pattern table 1
$2000-$23FF	$0400	Nametable 0
$2400-$27FF	$0400	Nametable 1
$2800-$2BFF	$0400	Nametable 2
$2C00-$2FFF	$0400	Nametable 3
$3000-$3EFF	$0F00	Mirrors of $2000-$2EFF
$3F00-$3F1F	$0020	Palette RAM indexes
$3F20-$3FFF	$00E0	Mirrors of $3F00-$3F1F
*/
uint8_t ppu_read_data(void) {
    uint16_t addr = ppu.addr.value; 
    ppu_vram_addr_increment(); 
    uint8_t result = ppu.buffer; 
    if (addr < 0x2000) {
        /// pattern table 0 and 1 
        result = ppu.buffer; 
        ppu.buffer = ppu.chr_rom[addr]; 
    } 
    else if (0x2000 <= addr && addr < 0x3000) {
        /// nametables 0, 1, 2, 3 
        result = ppu.buffer; 
        ppu.buffer = ppu.vram[ppu_mirror_vram_addr(addr)]; 
    } 
    else if (0x3000 <= addr && addr < 0x3f00) {
        /// mirrors of 0x2000 - 0x2eff (..<0x2f00)
        addr &= (UINT16_C(0xefff)); 
        result = ppu.buffer; 
        ppu.buffer = ppu.vram[ppu_mirror_vram_addr(addr)]; 
        // panic("reading unknown ppu address %04x\n", addr); 
    } 
    ////// NOTE: PPU address 0x3f00 - 0x3fff is not configurable -- always mapped to internal palette control 
    else {
        uint16_t relative_addr = (addr - UINT16_C(0x3f00)) % UINT16_C(32); // addr = 0x3f00 - 0x3f1f 
        if (relative_addr == 0x10 || relative_addr == 0x14 || relative_addr == 0x18 || relative_addr == 0x1c) {
            result = ppu.palette_table[relative_addr-0x10]; 
        } else {
            result = ppu.palette_table[relative_addr]; 
        }
    }
    return result; 
}
/**
Address range	Size	Description
$0000-$0FFF	$1000	Pattern table 0
$1000-$1FFF	$1000	Pattern table 1
$2000-$23FF	$0400	Nametable 0
$2400-$27FF	$0400	Nametable 1
$2800-$2BFF	$0400	Nametable 2
$2C00-$2FFF	$0400	Nametable 3
$3000-$3EFF	$0F00	Mirrors of $2000-$2EFF
$3F00-$3F1F	$0020	Palette RAM indexes
$3F20-$3FFF	$00E0	Mirrors of $3F00-$3F1F
*/
void ppu_write_data(uint8_t data) {
    uint16_t addr = ppu.addr.value; 
    ppu_vram_addr_increment(); 
    if (addr < 0x2000) {
        panic("attempting to write to chr rom address %04x\n", addr); 
    } 
    else if (0x2000 <= addr && addr < 0x3000) {
        ppu.vram[ppu_mirror_vram_addr(addr)] = data; 
    } 
    else if (0x3000 <= addr && addr < 0x3f00) {
        // printf("writing to unknown ppu address %04x\n", addr); 
        /// mirrors of 0x2000 - 0x2eff (..<0x2f00)
        addr &= (UINT16_C(0xefff)); 
        ppu.vram[ppu_mirror_vram_addr(addr)] = data;
        // panic("writing to unknown ppu address %04x\n", addr); 
    } 
    ////// NOTE: PPU address 0x3f00 - 0x3fff is not configurable -- always mapped to internal palette control 
    else {
        uint16_t relative_addr = (addr - UINT16_C(0x3f00)) % UINT16_C(32); // addr = 0x3f00 - 0x3f1f 
        if (relative_addr == 0x10 || relative_addr == 0x14 || relative_addr == 0x18 || relative_addr == 0x1c) {
            ppu.palette_table[relative_addr-0x10] = data; 
        } else {
            ppu.palette_table[relative_addr] = data; 
        }
    }
}
struct rgb ppu_mask_get_color_emphasis(void) {
    struct rgb result; 
    result.r = !!(ppu.mask & ppu_mask_emphasize_red);
    result.g = !!(ppu.mask & ppu_mask_emphasize_green);
    result.b = !!(ppu.mask & ppu_mask_emphasize_blue);
    return result; 
}
void ppu_write_scroll(uint8_t value) {
    if (ppu.scroll.reading_y) {
        ppu.scroll.y = value; 
    } else {
        ppu.scroll.x = value; 
    }
    ppu.scroll.reading_y = !ppu.scroll.reading_y; 
}
void ppu_reset_scroll_state(void) {
    ppu.scroll.reading_y = 0; 
}
void ppu_write_ctrl(uint8_t value) {
/*
    In addition to scanline position, PPU would immidiately trigger NMI if both of these conditions are met: 
    - PPU is VBLANK state
    - "Generate NMI" bit in the controll Register is updated from 0 to 1.
*/
    uint8_t prev_generate_nmi = (ppu.ctrl & ppu_ctrl_generate_nmi); 
    ppu.ctrl = value; 
    if ((ppu.status & ppu_status_vblank_started) && (!prev_generate_nmi) && (value & ppu_ctrl_generate_nmi)) {
        ppu.nmi_raised = 1; 
    }
}
void ppu_write_mask(uint8_t value) {
    ppu.mask = value; 
}
void ppu_write_oam_addr(uint8_t value) {
    ppu.oam_addr = value; 
}
uint8_t ppu_read_oam_data(void) {
    return ppu.oam_data[ppu.oam_addr];  
}
void ppu_write_oam_data(uint8_t value) {
    ppu.oam_data[ppu.oam_addr] = value; 
    ppu.oam_addr += 1; 
}
uint8_t ppu_read_status(void) {
    uint8_t s = ppu.status; 
    clear_mask_inplace(ppu.status, ppu_status_vblank_started); 
    ppu_reset_addr_state(); 
    ppu_reset_scroll_state(); 
    return s; 
}
void ppu_oam_dma(uint8_t *data, unsigned data_size) {
    for (unsigned i = 0; i < data_size; ++i) {
        ppu.oam_data[ppu.oam_addr] = data[i]; 
        ppu.oam_addr += 1; 
    }
}
