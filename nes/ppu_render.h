#pragma once 
#include "sdk.h"
/// advance the PPU cycles, each cycle draws a pixel when it's in drawing state 
bool ppu_tick_cycles(unsigned ppu_cycles); 

/// render a frame 
void ppu_render_frame(void); 
struct ppu_device; 

void ppu_draw_background_tile(struct ppu_device *ppu, unsigned bank, unsigned tile_idx, int tile_x, int tile_y);  
void ppu_get_palette_for_background_tile(uint8_t *palette_color_indices, unsigned tilex, unsigned tiley); 
void ppu_get_palette_for_sprite_tile(uint8_t *palette_color_indices, unsigned palette_index); 

extern struct rgb system_palette[64]; 


