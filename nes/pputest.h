#pragma once 

#include "ppu.h"
#include "sdk.h"
static uint8_t empty_chr[256]; 
static void test_ppu_vram_write(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal); 
    ppu_write_addr(0x23);
    ppu_write_addr(0x05); 
    ppu_write_data(0x66); 
    assert(ppu.vram[0x0305] == 0x66); 
}
static void test_ppu_vram_reads(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal); 
    ppu_write_ctrl(0); 
    ppu.vram[0x0305] = 0x66; 
    ppu_write_addr(0x23);
    ppu_write_addr(0x05); 
    ppu_read_data(); 
    assert(ppu.addr.value == 0x2306); 
    assert(ppu_read_data() == 0x66); 
}
static void test_ppu_vram_reads_cross_page(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal); 
    ppu_write_ctrl(0); 
    ppu.vram[0x01ff] = 0x66; 
    ppu.vram[0x0200] = 0x77; 
    ppu_write_addr(0x21); 
    ppu_write_addr(0xff); 
    ppu_read_data(); 
    assert(ppu_read_data() == 0x66); 
    assert(ppu_read_data() == 0x77); 
}
static void test_ppu_vram_reads_step_32(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal); 
    ppu_write_ctrl(0b100); 
    ppu.vram[0x01ff] = 0x66;
    ppu.vram[0x01ff + 32] = 0x77;
    ppu.vram[0x01ff + 64] = 0x88;
    ppu_write_addr(0x21); 
    ppu_write_addr(0xff); 
    ppu_read_data(); 
    assert(ppu_read_data() == 0x66);
    assert(ppu_read_data() == 0x77);
    assert(ppu_read_data() == 0x88);
}
static void test_vram_horizontal_mirror(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal); 
    ppu_write_addr(0x24); 
    ppu_write_addr(0x05); 
    ppu_write_data(0x66); 

    ppu_write_addr(0x28); 
    ppu_write_addr(0x05); 
    ppu_write_data(0x77); 

    ppu_write_addr(0x20); 
    ppu_write_addr(0x05); 
    ppu_read_data(); 
    assert(ppu_read_data() == 0x66); 

    ppu_write_addr(0x2c); 
    ppu_write_addr(0x05); 
    ppu_read_data(); 
    assert(ppu_read_data() == 0x77); 
}
static void test_vram_vertical_mirror(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_vertical);
    ppu_write_addr(0x20);
    ppu_write_addr(0x05);
    ppu_write_data(0x66); 

    ppu_write_addr(0x2C);
    ppu_write_addr(0x05);
    ppu_write_data(0x77); 

    ppu_write_addr(0x28);
    ppu_write_addr(0x05);
    ppu_read_data(); 
    assert(ppu_read_data() == 0x66); 

    ppu_write_addr(0x24);
    ppu_write_addr(0x05);
    ppu_read_data(); 
    assert(ppu_read_data() == 0x77); 
}
static void test_read_status_resets_latch(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal);
    ppu.vram[0x0305] = 0x66;
    ppu_write_addr(0x21);
    ppu_write_addr(0x23);
    ppu_write_addr(0x05);
    ppu_read_data();
    assert(ppu_read_data() != 0x66); 

    ppu_read_status(); 
    ppu_write_addr(0x23); 
    ppu_write_addr(0x05); 

    ppu_read_data(); 
    assert(ppu_read_data() == 0x66); 
}
static void test_ppu_vram_mirroring(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal);
    ppu_write_ctrl(0); 
    ppu.vram[0x0305] = 0x66;
    ppu_write_addr(0x63); 
    ppu_write_addr(0x05); 
    ppu_read_data(); 
    assert(ppu_read_data() == 0x66); 
}
static void test_read_status_resets_vblank(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal);
    set_mask_inplace(ppu.status, ppu_status_vblank_started); 
    uint8_t s = ppu_read_status(); 
    assert((s >> 7) == 1); 
    assert((ppu.status >> 7) == 0); 
}
static void test_oam_read_write(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal);
    ppu_write_oam_addr(0x10);
    ppu_write_oam_data(0x66);
    ppu_write_oam_data(0x77);

    ppu_write_oam_addr(0x10);
    assert(ppu_read_oam_data() == 0x66);

    ppu_write_oam_addr(0x11);
    assert(ppu_read_oam_data() == 0x77);
}
static void test_oam_dma(void) {
    ppu_init((uint8_t*)empty_chr, 256, rom_mirror_horizontal);
    uint8_t data[256]; 
    memset((uint8_t*)data, 0x66, 256); 
    data[0] = 0x77;
    data[255] = 0x88;

    ppu_write_oam_addr(0x10);
    ppu_oam_dma(data, 256);

    ppu_write_oam_addr(0xf); 
    assert(ppu_read_oam_data() == 0x88);

    ppu_write_oam_addr(0x10);
    ppu_write_oam_addr(0x77);
    ppu_write_oam_addr(0x11);
    ppu_write_oam_addr(0x66);
}
static void test_ppu(void) {
    test_ppu_vram_write(); 
    test_ppu_vram_reads(); 
    test_ppu_vram_reads_cross_page(); 
    test_ppu_vram_reads_step_32(); 
    test_vram_horizontal_mirror(); 
    test_vram_vertical_mirror(); 
    test_read_status_resets_latch(); 
    test_ppu_vram_mirroring(); 
    test_read_status_resets_vblank(); 
    test_oam_read_write(); 
    test_oam_dma(); 
}








