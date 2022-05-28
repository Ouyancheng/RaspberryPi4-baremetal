#include "ppu.h"
#include "display_interface.h"
struct ppu_device ppu; 
struct rgb system_palette[64] = {
    {0xFF, 0x80, 0x80, 0x80}, {0xFF, 0x00, 0x3D, 0xA6}, {0xFF, 0x00, 0x12, 0xB0}, {0xFF, 0x44, 0x00, 0x96}, {0xFF, 0xA1, 0x00, 0x5E},
    {0xFF, 0xC7, 0x00, 0x28}, {0xFF, 0xBA, 0x06, 0x00}, {0xFF, 0x8C, 0x17, 0x00}, {0xFF, 0x5C, 0x2F, 0x00}, {0xFF, 0x10, 0x45, 0x00},
    {0xFF, 0x05, 0x4A, 0x00}, {0xFF, 0x00, 0x47, 0x2E}, {0xFF, 0x00, 0x41, 0x66}, {0xFF, 0x00, 0x00, 0x00}, {0xFF, 0x05, 0x05, 0x05},
    {0xFF, 0x05, 0x05, 0x05}, {0xFF, 0xC7, 0xC7, 0xC7}, {0xFF, 0x00, 0x77, 0xFF}, {0xFF, 0x21, 0x55, 0xFF}, {0xFF, 0x82, 0x37, 0xFA},
    {0xFF, 0xEB, 0x2F, 0xB5}, {0xFF, 0xFF, 0x29, 0x50}, {0xFF, 0xFF, 0x22, 0x00}, {0xFF, 0xD6, 0x32, 0x00}, {0xFF, 0xC4, 0x62, 0x00},
    {0xFF, 0x35, 0x80, 0x00}, {0xFF, 0x05, 0x8F, 0x00}, {0xFF, 0x00, 0x8A, 0x55}, {0xFF, 0x00, 0x99, 0xCC}, {0xFF, 0x21, 0x21, 0x21},
    {0xFF, 0x09, 0x09, 0x09}, {0xFF, 0x09, 0x09, 0x09}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0x0F, 0xD7, 0xFF}, {0xFF, 0x69, 0xA2, 0xFF},
    {0xFF, 0xD4, 0x80, 0xFF}, {0xFF, 0xFF, 0x45, 0xF3}, {0xFF, 0xFF, 0x61, 0x8B}, {0xFF, 0xFF, 0x88, 0x33}, {0xFF, 0xFF, 0x9C, 0x12},
    {0xFF, 0xFA, 0xBC, 0x20}, {0xFF, 0x9F, 0xE3, 0x0E}, {0xFF, 0x2B, 0xF0, 0x35}, {0xFF, 0x0C, 0xF0, 0xA4}, {0xFF, 0x05, 0xFB, 0xFF},
    {0xFF, 0x5E, 0x5E, 0x5E}, {0xFF, 0x0D, 0x0D, 0x0D}, {0xFF, 0x0D, 0x0D, 0x0D}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xA6, 0xFC, 0xFF},
    {0xFF, 0xB3, 0xEC, 0xFF}, {0xFF, 0xDA, 0xAB, 0xEB}, {0xFF, 0xFF, 0xA8, 0xF9}, {0xFF, 0xFF, 0xAB, 0xB3}, {0xFF, 0xFF, 0xD2, 0xB0},
    {0xFF, 0xFF, 0xEF, 0xA6}, {0xFF, 0xFF, 0xF7, 0x9C}, {0xFF, 0xD7, 0xE8, 0x95}, {0xFF, 0xA6, 0xED, 0xAF}, {0xFF, 0xA2, 0xF2, 0xDA},
    {0xFF, 0x99, 0xFF, 0xFC}, {0xFF, 0xDD, 0xDD, 0xDD}, {0xFF, 0x11, 0x11, 0x11}, {0xFF, 0x11, 0x11, 0x11}
};

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
    ppu.cycles = 0; 
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
        // printf("addr=%04x chrom_size=%zu\n", addr, ppu.chr_rom_size); 
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
bool ppu_tick_cycles(unsigned ppu_cycles) {
    // printf("ppu_tick %u scanline=%u\n", ppu.cycles, ppu.scanline);
    ppu.cycles += ppu_cycles; 
    if (ppu.cycles >= 341) {
        ppu.cycles -= 341; 
        ppu.scanline += 1; 
        if (ppu.scanline == 241) {
            // printf("scanline = %u\n", ppu.scanline); 
            if (ppu.ctrl & ppu_ctrl_generate_nmi) {
                ppu.nmi_raised = 1; 
            }
            set_mask_inplace(ppu.status, ppu_status_vblank_started); 
            clear_mask_inplace(ppu.status, ppu_status_sprite_zero_hit); 
        }
        if (ppu.scanline >= 262) {
            ppu.scanline = 0; 
            // printf("scanline = %u\n", ppu.scanline); 
            ppu.nmi_raised = 0; 
            clear_mask_inplace(ppu.status, ppu_status_sprite_zero_hit); 
            clear_mask_inplace(ppu.status, ppu_status_vblank_started); 
            return true; 
        }
    }
    return false; 
}
void ppu_draw_background_tile(uint8_t *chr_rom, size_t chr_rom_size, unsigned bank, unsigned ntile, int tilex, int tiley) {
    unsigned bank_offset = bank * 0x1000u; 
    uint8_t *tile = chr_rom + bank_offset + ntile * 16;
    uint8_t background_palette_color[4]; 
    ppu_get_palette_for_background_tile((uint8_t*)background_palette_color, tilex, tiley); 
    for (int row = 0; row < 8; ++row) {
        uint8_t upper = tile[row];
        uint8_t lower = tile[row + 8];
        for (int column = 7; column >= 0; --column) {
            uint8_t val = ((upper & 1) << 1 | (lower & 1)) & 0b11; 
            upper >>= 1;
            lower >>= 1; 
            struct rgb color; 
            if (val == 0) {
                color = system_palette[ppu.palette_table[0]]; 
            } else if (val <= 3) {
                color = system_palette[background_palette_color[val]]; 
            } else {
                panic("value is out of 0..3\n"); 
            }
            display_set_pixel(column+tilex*8, row+tiley*8, color); 
        }
   }
}
#define RENDER_BACKGROUND 1 
#define RENDER_SPRITE 1
void ppu_render_frame(void) {
#if RENDER_BACKGROUND
    /***** Don't know what goes wrong here...... ******/
    // unsigned background_bank = (((ppu.ctrl & ppu_ctrl_background_pattern_addr) == 0) ? 0 : 1); 
    // for (unsigned i = 0; i < 960; ++i) {
    //     unsigned x = i % 32; 
    //     unsigned y = i / 32; 
    //     uint16_t ntile = ppu.vram[i]; 
    //     ppu_draw_background_tile(ppu.chr_rom, ppu.chr_rom_size, background_bank, ntile, x, y); 
    // }
    unsigned background_bank = (((ppu.ctrl & ppu_ctrl_background_pattern_addr) != 0));
    for (unsigned i = 0; i < 960; ++i) {
        uint16_t tile_idx = ppu.vram[i];
        unsigned tile_x = i % 32;
        unsigned tile_y = i / 32;
        uint8_t *tile = ppu.chr_rom + (background_bank*0x1000 + tile_idx * 16); 
        uint8_t background_palette[4]; 
        ppu_get_palette_for_background_tile(background_palette, tile_x, tile_y); 
        for (int y = 0; y < 8; ++y) {
            uint8_t upper = tile[y];
            uint8_t lower = tile[y + 8];
            for (int x = 7; x >= 0; --x) {
                uint8_t value = (1 & lower) << 1 | (1 & upper);
                upper >>= 1;
                lower >>= 1;
                struct rgb color; 
                color = system_palette[background_palette[value]]; 
                display_set_pixel(tile_x * 8 + x, tile_y * 8 + y, color); 
            }
        }
    }
#endif 
#if RENDER_SPRITE
    // display_render_frame(); 
    // return; 
    unsigned sprite_bank = ((ppu.ctrl & ppu_ctrl_sprite_pattern_addr) != 0); 
    // draw sprite 
    for (int i = 256-4; i >= 0; i -= 4) {
        unsigned sprite_tile_idx = (unsigned)ppu.oam_data[i+1]; 
        /// another strange thing... 
        unsigned sprite_tile_x = (unsigned)ppu.oam_data[i+3]; 
        unsigned sprite_tile_y = (unsigned)ppu.oam_data[i]; 
        bool flip_vertical = (((ppu.oam_data[i+2] >> 7) & 1)); 
        bool flip_horizontal = (((ppu.oam_data[i+2] >> 6) & 1)); 
        unsigned palette_index = ppu.oam_data[i+2] & 0b11; 
        uint8_t sprite_palette_colors[4]; 
        ppu_get_palette_for_sprite_tile((uint8_t*)sprite_palette_colors, palette_index); 
        uint8_t *sprite_tile = ppu.chr_rom + sprite_bank * 0x1000 + sprite_tile_idx * 16; 
        for (int y = 0; y < 8; ++y) {
            unsigned upper = sprite_tile[y]; 
            unsigned lower = sprite_tile[y+8]; 
            for (int x = 7; x >= 0; --x) {
                uint8_t val = (((lower & 1) << 1) | (upper & 1)); 
                upper >>= 1; 
                lower >>= 1; 
                struct rgb color; 
                if (val == 0) {
                    continue; 
                } else {
                    color = system_palette[sprite_palette_colors[val]]; 
                }
                unsigned xcoord = (flip_horizontal ? (sprite_tile_x + 7 - x) : sprite_tile_x + x); 
                unsigned ycoord = (flip_vertical ? (sprite_tile_y + 7 - y) : sprite_tile_y + y); 
                display_set_pixel(xcoord, ycoord, color); 
            }
        }
    }
#endif 
    display_render_frame(); 
}

void ppu_get_palette_for_background_tile(uint8_t *palette_color_indices, unsigned tilex, unsigned tiley) {
    /// strange way to encode the palette information: 
    /// a screen holds 32 * 30 = 960 tiles 
    /// a meta block of tile is 4 * 4 tiles (an entry in the attribute table)
    /// a small block of tile is 2 * 2 tiles (two bits in the block attribute)
    /// and the whole small block will share the same palette defined by that two bits 
    unsigned attribute_table_index = (tiley / 4) * (32/4) + tilex / 4; 
    uint8_t block_attribute = ppu.vram[0x3c0 + attribute_table_index]; 
    unsigned attr_2bit = ((block_attribute >> ((tilex % 4 / 2)*2)) >> ((tiley % 4 / 2)*4)) & 0b11; 
    unsigned palette_index = (unsigned)attr_2bit * 4 + 1; 
    palette_color_indices[0] = ppu.palette_table[0]; 
    palette_color_indices[1] = ppu.palette_table[palette_index]; 
    palette_color_indices[2] = ppu.palette_table[palette_index+1]; 
    palette_color_indices[3] = ppu.palette_table[palette_index+2]; 
}
void ppu_get_palette_for_sprite_tile(uint8_t *palette_color_indices, unsigned palette_index) {
    unsigned palette_start = palette_index * 4 + 16 + 1; 
    palette_color_indices[0] = 0; 
    palette_color_indices[1] = ppu.palette_table[palette_start]; 
    palette_color_indices[2] = ppu.palette_table[palette_start+1]; 
    palette_color_indices[3] = ppu.palette_table[palette_start+2]; 
}

