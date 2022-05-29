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




////// TODO: render pixel on a tick!!!!!!
////// THIS IS OVERLY SIMPLIFIED!!!!!! 
bool ppu_tick_cycles(unsigned ppu_cycles) {
    ppu.scanline_cycles += ppu_cycles; 
    if (ppu.scanline_cycles >= 341) {
        if (ppu_sprite0_hit(ppu.scanline_cycles)) {
            set_mask_inplace(ppu.status, ppu_status_sprite_zero_hit); // simplified 
        }
        ppu.scanline_cycles -= 341; 
        ppu.scanline += 1; 
        if (ppu.scanline == 241) {
            if (ppu.ctrl & ppu_ctrl_generate_nmi) {
                ppu.nmi_raised = 1; 
            }
            set_mask_inplace(ppu.status, ppu_status_vblank_started); 
            clear_mask_inplace(ppu.status, ppu_status_sprite_zero_hit); 
        }
        if (ppu.scanline >= 262) {
            ppu.scanline = 0; 
            ppu.nmi_raised = 0; 
            clear_mask_inplace(ppu.status, ppu_status_sprite_zero_hit); 
            clear_mask_inplace(ppu.status, ppu_status_vblank_started); 
            return true; 
        }
    }
    return false; 
}


void ppu_get_palette_for_background_tile(uint8_t *palette_color_indices, unsigned tile_x, unsigned tile_y, uint8_t *attribute_table) {
    /// strange way to encode the palette information: 
    /// a screen holds 32 * 30 = 960 tiles 
    /// a meta block of tile is 4 * 4 tiles (an entry in the attribute table)
    /// a small block of tile is 2 * 2 tiles (two bits in the block attribute)
    /// and the whole small block will share the same palette defined by that two bits 
    unsigned attribute_table_index = (tile_y / 4) * (32/4) + tile_x / 4; 
    uint8_t block_attribute = attribute_table[attribute_table_index]; // ppu.vram[960 + attribute_table_index]; 
    unsigned attr_2bit = ((block_attribute >> ((tile_x % 4 / 2)*2)) >> ((tile_y % 4 / 2)*4)) & 0b11; 
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
    ppu_get_palette_for_background_tile(background_palette, tile_x, tile_y, (uint8_t*)ppu->vram); 
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
    // unsigned background_bank = (((ppu.ctrl & ppu_ctrl_background_pattern_addr) != 0));
    // for (unsigned i = 0; i < 960; ++i) {
    //     uint16_t tile_idx = ppu.vram[i];
    //     unsigned tile_x = i % 32;
    //     unsigned tile_y = i / 32;
    //     ppu_draw_background_tile(&ppu, background_bank, tile_idx, tile_x, tile_y); 
    // }


    unsigned scroll_x = (unsigned)ppu.scroll.x; 
    unsigned scroll_y = (unsigned)ppu.scroll.y; 
    uint16_t base_nametable_addr = UINT16_C(0x2000) + (ppu.ctrl & (ppu_ctrl_nametable1 | ppu_ctrl_nametable2)) * UINT16_C(0x0400);
    uint8_t *primary_nametable = NULL; 
    uint8_t *secondary_nametable = NULL; 
    /// TODO: handle more mirroring!!! 
    switch (base_nametable_addr) {
        case 0x2000: 
            primary_nametable = (uint8_t*)ppu.vram; 
            secondary_nametable = primary_nametable + 0x0400; 
            break; 
        case 0x2400:
            if (ppu.mirror == rom_mirror_vertical) {
                secondary_nametable = (uint8_t*)ppu.vram; 
                primary_nametable = secondary_nametable + 0x0400; 
            } else {
                primary_nametable = (uint8_t*)ppu.vram; 
                secondary_nametable = primary_nametable + 0x0400; 
            }
            break; 
        case 0x2800:
            if (ppu.mirror == rom_mirror_vertical) {
                primary_nametable = (uint8_t*)ppu.vram; 
                secondary_nametable = primary_nametable + 0x0400; 
            } else {
                secondary_nametable = (uint8_t*)ppu.vram; 
                primary_nametable = secondary_nametable + 0x0400; 
            }
            break; 
        case 0x2C00: 
            secondary_nametable = (uint8_t*)ppu.vram; 
            primary_nametable = secondary_nametable + 0x0400; 
            break; 
        default: 
            panic("base name table address = %04x\n", base_nametable_addr); 
            break; 
    }
    ppu_render_nametable_with_scroll(
        primary_nametable, 
        (unsigned)scroll_x, (unsigned)scroll_y, 256, 240, 
        -((int)scroll_x), -((int)scroll_y)
    ); 
    // horizontal scroll 
    if (scroll_x > 0) {
        ppu_render_nametable_with_scroll(
            secondary_nametable, 
            0, 0, scroll_x, 240, 
            (int)(256-scroll_x), 0
        ); 
    }
    // vertical scroll 
    else if (scroll_y > 0) {
        ppu_render_nametable_with_scroll(
            secondary_nametable, 
            0, 0, 256, scroll_y, 
            0, (int)(240-scroll_y) 
        ); 
    } 
    else {
        // printf("stationary\n"); 
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


/*
From NesDev: 
Sprite zero hits
Sprites are conventionally numbered 0 to 63. 
Sprite 0 is the sprite controlled by OAM addresses $00-$03, 
sprite 1 is controlled by $04-$07, ..., and sprite 63 is controlled by $FC-$FF.

While the PPU is drawing the picture, 
**when an opaque pixel of sprite 0 overlaps an opaque pixel of the background**, 
    this is a sprite zero hit. 
The PPU detects this condition and sets bit 6 of PPUSTATUS ($2002) to 1 starting at this pixel, 
    letting the CPU know how far along the PPU is in drawing the picture.

Sprite 0 hit does not happen:
- If background or sprite rendering is disabled in PPUMASK ($2001)
- At x=0 to x=7 if the left-side clipping window is enabled (if bit 2 or bit 1 of PPUMASK is 0).
- At x=255, for an obscure reason related to the pixel pipeline.
- At any pixel where the background or sprite pixel is transparent (2-bit color index from the CHR pattern is %00).
- If sprite 0 hit has already occurred this frame. 
    Bit 6 of PPUSTATUS ($2002) is cleared to 0 at dot 1 of the pre-render line. 
    This means only the first sprite 0 hit in a frame can be detected.

Sprite 0 hit happens regardless of the following:
- Sprite priority. 
    Sprite 0 can still hit the background from behind.
- The pixel colors. 
    Only the CHR pattern bits are relevant, 
    not the actual rendered colors, 
    and any CHR color index except %00 is considered opaque.
- The palette. 
    The contents of the palette are irrelevant to sprite 0 hits. 
    For example: a black ($0F) sprite pixel can hit a black ($0F) background as long as neither is the transparent color index %00.
- The PAL PPU blanking on the left and right edges at x=0, x=1, and x=254 (see Overscan).
*/
bool ppu_sprite0_hit(unsigned current_scanline_cycle) {
    ////// TODO: THIS IS Overly simplified!!! 
    if (!(ppu.mask & ppu_mask_show_sprites)) {
        return false; 
    }
    unsigned sprite_x = (unsigned)ppu.oam_data[3]; 
    unsigned sprite_y = (unsigned)ppu.oam_data[0]; 
    return ppu.scanline == sprite_y && sprite_x <= ppu.scanline_cycles; 
}

void ppu_render_nametable_with_scroll(
    uint8_t *nametable, 
    unsigned viewport_upperleft_x,   unsigned viewport_upperleft_y, 
    unsigned viewport_bottomright_x, unsigned viewport_bottomright_y, 
    int viewport_screen_shift_x,     int viewport_screen_shift_y
) {
    unsigned bank = ((ppu.ctrl & ppu_ctrl_background_pattern_addr) != 0);
    uint8_t *attribute_table = nametable + 960; 
    for (int i = 0; i < 960; ++i) {
        uint16_t tile_idx = nametable[i];
        unsigned tile_x = i % 32;
        unsigned tile_y = i / 32;
        uint8_t *tile = ppu.chr_rom + (bank*0x1000 + tile_idx * 16); 
        uint8_t background_palette[4]; 
        ppu_get_palette_for_background_tile(background_palette, tile_x, tile_y, attribute_table); 
        for (int y = 0; y < 8; ++y) {
            uint8_t upper = tile[y];
            uint8_t lower = tile[y + 8];
            for (int x = 7; x >= 0; --x) {
                uint8_t value = (1 & lower) << 1 | (1 & upper);
                upper >>= 1;
                lower >>= 1;
                struct rgb color; 
                color = system_palette[background_palette[value]]; 
                unsigned xcoord = (unsigned)tile_x * 8 + (unsigned)x; 
                unsigned ycoord = (unsigned)tile_y * 8 + (unsigned)y; 
                if (viewport_upperleft_x <= xcoord && xcoord < viewport_bottomright_x && 
                    viewport_upperleft_y <= ycoord && ycoord < viewport_bottomright_y
                ) {
                    display_set_pixel((int)xcoord + viewport_screen_shift_x, (int)ycoord + viewport_screen_shift_y, color); 
                }
            }
        }
    }

}

