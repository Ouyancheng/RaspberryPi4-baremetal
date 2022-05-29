#pragma once 
#include "sdk.h"
/// advance the PPU cycles, each cycle draws a pixel when it's in drawing state 
bool ppu_tick_cycles(unsigned ppu_cycles); 

/// render a frame 
void ppu_render_frame(void); 
struct ppu_device; 

void ppu_draw_background_tile(struct ppu_device *ppu, unsigned bank, unsigned tile_idx, int tile_x, int tile_y);  
void ppu_get_palette_for_background_tile(uint8_t *palette_color_indices, unsigned tile_x, unsigned tile_y, uint8_t *attribute_table); 
void ppu_get_palette_for_sprite_tile(uint8_t *palette_color_indices, unsigned palette_index); 
/// reports whether sprite 0 has already rendered 
bool ppu_sprite0_hit(unsigned current_scanline_cycle); 
extern struct rgb system_palette[64]; 


void ppu_render_nametable_with_scroll(
    uint8_t *nametable, 
    unsigned viewport_upperleft_x, unsigned viewport_upperleft_y, 
    unsigned viewport_bottomright_x, unsigned viewport_bottomright_y, 
    int viewport_screen_shift_x, int viewport_screen_shift_y
); 


