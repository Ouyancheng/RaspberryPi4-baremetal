#include "sdk.h"
#include "cpu.h"
#include "cpu_debug.h"
#include "ppu.h"
#include "controller.h"
#include "nes_state.h"
#include "display_interface.h"
#include "controller_interface.h" 
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
#ifdef PLATFORM_PC 
void test_cpu_rom(const char *rom_path) {
    bool test_mode = false; 
    if (!rom_path) {
        test_mode = true; 
        rom_path = "../testroms/nestest.nes"; 
    }
    nes_memory_init(); 
    unsigned romsize = load_file((uint8_t*)system_memory.rom_binary, 1048576, rom_path); 
    // if (romsize) {
    //     FILE *c_array_dump = fopen("romdata.c", "wb"); 
    //     fprintf(c_array_dump, "unsigned romsize = %d;\n", romsize); 
    //     fprintf(c_array_dump, "char romdata[] = {\n    ");
    //     for (unsigned i = 0; i < romsize; ++i) {
    //         fprintf(c_array_dump , "0x%02x, ", system_memory.rom_binary[i]); 
    //     }
    //     fprintf(c_array_dump, "\n};\n");
    //     fclose(c_array_dump); 
    // }
    display_init("PPU render window", NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT, 2, 2); 
    load_rom((uint8_t*)system_memory.rom_binary, romsize); 
    controller_init(&controller1); 
    controller_init(&controller2); 
    bus_init(); 
    ppu_init(); 
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


#ifdef PLATFORM_RPI 
extern unsigned romsize; 
extern char romdata[]; 
int nes_run(void) {
    printf("hello!!! please close this session and upload a rom file in another session~\n"); 
    nes_memory_init(); 
    unsigned romsize = load_file((uint8_t*)system_memory.rom_binary, 1048576, NULL); 
    if (!romsize) {
        panic("failed to get rom!\n"); 
    }
    display_init("tile", NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT, 3, 3); 
    printf("framebuffer initialized!\n"); 
    // printf("system_memory.rom_binary = %p, romdata = %p\n", (uint8_t*)system_memory.rom_binary, (uint8_t*)romdata);
    // for (unsigned i = 0; i < romsize; ++i) {
    //     // printf("%u\n", i); 
    //     system_memory.rom_binary[i] = (uint8_t)romdata[i]; 
    //     asm volatile ("nop":::"memory"); 
    //     dev_barrier(); 
    // }
    printf("about to install rom!\n"); 
    load_rom((uint8_t*)system_memory.rom_binary, romsize); 
    printf("rom mapper = %u\n", (unsigned)rom.mapper); 
    dev_barrier();
    printf("about to initiate bus\n"); 
    asm volatile ("nop":::"memory"); 
    dev_barrier();
    bus_init();  
    asm volatile ("nop":::"memory"); 
    printf("bus initiated!!!\n"); dev_barrier(); 
    ppu_init(); 
    printf("ppu initialized\n"); dev_barrier(); 
    dev_barrier(); 
    asm volatile ("nop":::"memory"); 
    if (rom.mapper > 0) {
        printf("mapper %u is not supported\n", (unsigned)rom.mapper); 
        dev_barrier(); 
        display_exit(); 
        exit(0); 
    }
    asm volatile ("nop":::"memory"); 
    printf("about to initiate controllers\n"); 
    dev_barrier(); 
    controller_init(&controller1); 
    controller_init(&controller2); 
    asm volatile ("nop":::"memory"); 
    printf("about to initiate cpu\n"); 
    dev_barrier();
    cpu_init(); 
    cpu_reset(); 
    asm volatile ("nop":::"memory"); 
    printf("about to run!\n"); 
    dev_barrier();
    asm volatile ("nop":::"memory"); 
    cpu_run(); 
    // cpu_run_with_callback(dump_cpu);
    display_exit(); 
    exit(0); 
    return 0; 
}
#endif 
