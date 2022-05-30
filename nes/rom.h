#pragma once 
#include "sdk.h"
#include "ppu.h"

struct nes_rom {
    uint8_t *prg_rom; 
    uint8_t *chr_rom; 
    size_t prg_rom_size; 
    size_t chr_rom_size; 
    uint8_t mapper; 
    enum nametable_mirror mirroring; 
};


void load_rom(uint8_t *bytes, size_t rom_size); 

