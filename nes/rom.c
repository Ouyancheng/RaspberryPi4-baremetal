#include "rom.h"
uint8_t rom_bytes[ROM_SIZE_MAX]; 
struct nes_rom load_rom(uint8_t *bytes, size_t rom_size) {
    struct nes_rom rom; 
    uint8_t ines1_tag[] = {0x4E, 0x45, 0x53, 0x1A};
    if (memcmp(bytes, (uint8_t*)ines1_tag, sizeof(ines1_tag)) != 0) {
        panic("Error: load_rom not a valid nes rom!\n"); 
    } 
    rom.mapper = ((bytes[7] & 0xF0) | (bytes[6] >> 4)); 
    uint8_t version = (bytes[7] >> 2) & 0x3; 
    if (version != 0) {
        panic("Error: version %d is not supported\n", version); 
    }
    uint8_t four_screen = bytes[6] & 0b1000;
    if (four_screen) {
        rom.mirroring = rom_mirror_four_screen;
    } else {
        rom.mirroring = (bytes[6] & 1) ? rom_mirror_vertical : rom_mirror_horizontal; 
    }
    rom.prg_rom_size = (size_t)bytes[4] * 16 * 1024; 
    rom.chr_rom_size = (size_t)bytes[5] * 8 * 1024; 
    if (bytes[6] & 0b100) {
        rom.prg_rom = bytes + 16 + 512; 
    } else {
        rom.prg_rom = bytes + 16; 
    }
    rom.chr_rom = rom.prg_rom + rom.prg_rom_size; 
    return rom; 
}
