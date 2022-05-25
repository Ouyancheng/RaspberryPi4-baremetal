#pragma once 
#include "sdk.h"

enum rom_mirror {
    rom_mirror_vertical, 
    rom_mirror_horizontal, 
    rom_mirror_four_screen 
};
struct nes_rom {
    uint8_t *prg_rom; 
    uint8_t *chr_rom; 
    size_t prg_rom_size; 
    size_t chr_rom_size; 
    uint8_t mapper; 
    enum rom_mirror mirroring; 
};
#define ROM_SIZE_MAX (1024*1024)
extern uint8_t rom_bytes[ROM_SIZE_MAX]; 
struct nes_rom load_rom(uint8_t *bytes, size_t rom_size); 

