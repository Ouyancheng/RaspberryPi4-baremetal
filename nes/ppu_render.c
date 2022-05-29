#include "ppu_render.h"
#include "ppu.h"
#include "display_interface.h"


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





bool ppu_tick_cycles(unsigned ppu_cycles) {
    ////// TODO: render pixel on a tick!!!!!!
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




void ppu_draw_background_tile(struct ppu_device *ppu, unsigned bank, unsigned tile_idx, int tile_x, int tile_y) {
    uint8_t *tile = ppu->chr_rom + (bank*0x1000 + tile_idx * 16); 
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

void ppu_draw_sprite_tile(struct ppu_device *ppu, unsigned sprite_bank, unsigned i_th_sprite) {
    unsigned i = i_th_sprite * 4; 
    unsigned sprite_tile_idx = (unsigned)ppu->oam_data[i+1]; 
    /// another strange thing... 
    unsigned sprite_tile_x = (unsigned)ppu->oam_data[i+3]; 
    unsigned sprite_tile_y = (unsigned)ppu->oam_data[i]; 
    bool flip_vertical = (((ppu->oam_data[i+2] >> 7) & 1)); 
    bool flip_horizontal = (((ppu->oam_data[i+2] >> 6) & 1)); 
    unsigned palette_index = ppu->oam_data[i+2] & 0b11; 
    uint8_t sprite_palette_colors[4]; 
    ppu_get_palette_for_sprite_tile((uint8_t*)sprite_palette_colors, palette_index); 
    uint8_t *sprite_tile = ppu->chr_rom + sprite_bank * 0x1000 + sprite_tile_idx * 16; 
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
#define RENDER_BACKGROUND 1 
#define RENDER_SPRITE 1
void ppu_render_frame(void) {
#if RENDER_BACKGROUND
    unsigned background_bank = (((ppu.ctrl & ppu_ctrl_background_pattern_addr) != 0));
    for (unsigned i = 0; i < 960; ++i) {
        uint16_t tile_idx = ppu.vram[i];
        unsigned tile_x = i % 32;
        unsigned tile_y = i / 32;
        ppu_draw_background_tile(&ppu, background_bank, tile_idx, tile_x, tile_y); 
    }
#endif 
#if RENDER_SPRITE
    unsigned sprite_bank = ((ppu.ctrl & ppu_ctrl_sprite_pattern_addr) != 0); 
    // draw sprite 
    for (int i = 63; i >= 0; i -= 1) {
        ppu_draw_sprite_tile(&ppu, sprite_bank, i); 
    }
#endif 
    display_render_frame(); 
}



