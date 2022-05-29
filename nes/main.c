#include "sdk.h"
#include "cpu.h"
#include "cpu_debug.h"
#include "ppu.h"
#include "controller.h"
#include "display_interface.h"
#include "controller_interface.h" 
#ifdef PLATFORM_UNIX 
void nmi_callback_render_frame(void) {
    ppu_render_frame(); 
    controller_handle_input(0); 
    uint32_t tick = get_current_time_ms(); 
    int latency = 0; 
    if ((latency = (1000 / FRAMERATE) - (int)(tick - last_frame_tick)) > 0) {
        delay_ms(latency);
    }
    last_frame_tick = get_current_time_ms();
}
void test_cpu_rom(const char *rom_path) {
    srand(0); 
    bool test_mode = false; 
    if (!rom_path) {
        test_mode = true; 
        rom_path = "../testroms/nestest.nes"; 
    }
    unsigned romsize = load_file((uint8_t*)rom_bytes, 1048576, rom_path); 
    display_init("tile", NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT, 3, 3); 
    struct nes_rom rom = load_rom((uint8_t*)rom_bytes, romsize); 
    controller_init(&controller1); 
    controller_init(&controller2); 
    bus_init(rom, NULL); 
    /// 
    // printf("romsize = %zu, prgsize = %zu, chrsize = %zu\n", romsize, rom.prg_rom_size, rom.chr_rom_size); 
    // for (int b = 0; b <= 0; ++b) {
    //     for (int tile = 0; tile < 256; ++tile) {
    //         ppu_draw_background_tile(&ppu, b, tile, (tile % 32), (tile / 32)); 
    //     }
    // }
    // display_render_frame(); 
    // while (1) {
    //     controller_handle_input(1000); 
    // }
    /// 

    if (test_mode) {
        cpu_init(); 
        cpu_reset(); 
        cpu.pc = 0xC000; // hack 
        cpu_run_with_callback(dump_cpu); 
        display_exit(); 
        return; 
    }
    printf("rom mapper = %u\n", (unsigned)rom.mapper); 
    if (rom.mapper > 0) {
        printf("mapper %u is not supported\n", (unsigned)rom.mapper); 
        display_exit(); 
        return; 
    }
    cpu_init(); 
    cpu_reset(); 
    cpu_run(); 
    // cpu_run_with_callback(dump_cpu);
    display_exit(); 
    printf("Done!!!\n"); 
    return; 
}
int main(int argc, const char *argv[]) {
    if (argc > 1) {
        test_cpu_rom(argv[1]);
    } else {
        test_cpu_rom(NULL); 
    }
    return 0; 
}

#endif 
