#pragma once 

#include "sdk.h"
#include "rom.h"

struct ppu_reg_addr {
    uint16_t value; 
    unsigned reading_lo; 
};
struct ppu_reg_scroll {
    uint8_t x; 
    uint8_t y; 
    unsigned reading_y; 
};
struct ppu_device {
    uint8_t *chr_rom; 
    size_t chr_rom_size; 
    uint8_t palette_table[32]; 
    uint8_t vram[2048]; 
    uint8_t oam_data[256]; 
    enum rom_mirror mirror; 
    struct ppu_reg_addr addr; 
    struct ppu_reg_scroll scroll; 
    uint8_t ctrl; 
    uint8_t mask; 
    uint8_t status; 
    uint8_t oam_addr; 
    uint8_t buffer; 
    unsigned cycles; 
    unsigned scanline; 
    unsigned nmi_raised; 
};

extern struct ppu_device ppu; 

enum ppu_control_flags {
    ppu_ctrl_nametable1              = 0b00000001, 
    ppu_ctrl_nametable2              = 0b00000010, 
    ppu_ctrl_vram_addr_increment     = 0b00000100, 
    ppu_ctrl_sprite_pattern_addr     = 0b00001000, 
    ppu_ctrl_background_pattern_addr = 0b00010000, 
    ppu_ctrl_sprite_size             = 0b00100000, 
    ppu_ctrl_master_slave_select     = 0b01000000, 
    ppu_ctrl_generate_nmi            = 0b10000000 
};

enum ppu_mask_flags {
    ppu_mask_greyscale               = 0b00000001,
    ppu_mask_leftmost_8px_background = 0b00000010,
    ppu_mask_leftmost_8px_sprite     = 0b00000100,
    ppu_mask_show_background         = 0b00001000,
    ppu_mask_show_sprites            = 0b00010000,
    ppu_mask_emphasize_red           = 0b00100000,
    ppu_mask_emphasize_green         = 0b01000000,
    ppu_mask_emphasize_blue          = 0b10000000
};

enum ppu_status_flags {
    ppu_status_unused1         = 0b00000001,
    ppu_status_unused2         = 0b00000010,
    ppu_status_unused3         = 0b00000100,
    ppu_status_unused4         = 0b00001000,
    ppu_status_unused5         = 0b00010000,
    ppu_status_sprite_overflow = 0b00100000,
    ppu_status_sprite_zero_hit = 0b01000000,
    ppu_status_vblank_started  = 0b10000000 
};


struct rgb {
    uint8_t a; 
    uint8_t b; 
    uint8_t g; 
    uint8_t r; 
};

void ppu_init(uint8_t *chr_rom, size_t chr_rom_size, enum rom_mirror mirror); 
void ppu_write_addr(uint8_t data); 
void ppu_reset_addr_state(void); 
uint8_t ppu_read_data(void); 
void ppu_write_data(uint8_t data); 
struct rgb ppu_mask_get_color_emphasis(void); 
void ppu_write_scroll(uint8_t value); 
void ppu_reset_scroll_state(void); 
void ppu_write_ctrl(uint8_t value);
void ppu_write_mask(uint8_t value);
void ppu_write_oam_addr(uint8_t value);
uint8_t ppu_read_oam_data(void);
void ppu_write_oam_data(uint8_t value);
uint8_t ppu_read_status(void); 
void ppu_oam_dma(uint8_t *data, unsigned data_size); 
bool ppu_tick_cycles(unsigned ppu_cycles); 






