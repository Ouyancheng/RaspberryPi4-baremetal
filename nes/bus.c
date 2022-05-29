#include "bus.h"
#include "ppu.h"
#include "controller.h"
#include "controller_interface.h"
#include "display_interface.h"
struct nes_bus bus; 

void bus_init(struct nes_rom bus_rom, void(*nmi_callback)(void)) {
    memset((uint8_t*)bus.cpu_ram, 0, CPU_RAM_SIZE); 
    bus.rom = bus_rom;
    bus.nmi_callback = nmi_callback; 
    bus.cycles = 0; 
    ppu_init(bus.rom.chr_rom, bus.rom.chr_rom_size, bus.rom.mirroring); 
}
uint8_t bus_read(uint16_t addr) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        return bus.cpu_ram[actual_addr]; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        switch (actual_addr) {
            case 0x2007:
                return ppu_read_data(); 
            case 0x2004: 
                // OAM data 
                return ppu_read_oam_data(); 
            case 0x2002: 
                // status 
                return ppu_read_status(); 
            default: 
                // panic("reading invalid ppu register %04x\n", addr); 
                break; 
        }
        return 0; 
    }
    else if (addr == 0x4014) {
        return 0; 
    }
    else if (0x4000 <= addr && addr <= 0x4015) {
        /// TODO: APU 
        return 0xff; 
    }
    else if (addr == 0x4016) {
        /// NOTE: 0x4016 controller 1 
        uint8_t ret = controller_read(&controller1); 
        // printf("polling controller 1 = %02x\n", ret); 
        return ret; 
    } 
    else if (addr == 0x4017) {
        /**
         * For some reason, controller 2 is always reading 0x01, which makes games that read both controllers fail to respond to input 
         * figuring out why...... 
         */
        /// NOTE: 0x4017 controller 2 
        // uint8_t ret = controller_read(&controller2); 
        // printf("polling controller 2 = %02x\n", ret); 
        // return ret; 
        return 0; 
    }
    else if (0x8000 <= addr) {
        return read_prg_rom(addr); 
    }
    else {
        // printf("reading from address %04x is ignored\n", addr); 
        return 0; 
    }
}

void bus_write(uint16_t addr, uint8_t value) {
    if (0x0000 <= addr && addr < 0x2000) {
        uint16_t actual_addr = addr & UINT16_C(0x07FF); 
        bus.cpu_ram[actual_addr] = value; 
    }
    else if (0x2000 <= addr && addr < 0x4000) {
        uint16_t actual_addr = addr & UINT16_C(0b0010000000000111);
        switch (actual_addr) {
            case 0x2000:
                // ctrl 
                ppu_write_ctrl(value); 
                break; 
            case 0x2001:
                // mask 
                ppu_write_mask(value); 
                break; 
            case 0x2002: 
                panic("the ppu status register is read-only! addr=%04x\n", addr); 
                break; 
            case 0x2003: 
                // OAM addr 
                ppu_write_oam_addr(value); 
                break; 
            case 0x2004: 
                // OAM data 
                ppu_write_oam_data(value); 
                break; 
            case 0x2005: 
                // scroll 
                // printf("write to scroll with %02x\n", value); 
                ppu_write_scroll(value); 
                break; 
            case 0x2006: 
                // ppu addr 
                ppu_write_addr(value); 
                break; 
            case 0x2007: 
                // ppu data 
                ppu_write_data(value); 
                break; 
            default: 
                panic("writing invalid ppu register %04x\n", addr); 
                break; 
        }
    } 
    else if (addr == 0x4014) {
        // oam dma!!! 
        uint8_t buffer[256];
        memset((uint8_t*)buffer, 0, 256); 
        uint16_t hi = ((uint16_t)value) << 8; 
        for (int i = 0; i < 256; ++i) {
            buffer[i] = bus_read(hi + (uint16_t)i); 
        }
        ppu_oam_dma((uint8_t*)buffer, 256); 
        /// NOTE: 
        /// there's weird timing issue, we  need to add back the cycles for DMA 
        unsigned compensation = ((bus.cycles % 2) ? 514 : 513); 
        // for (int i = 0; i < compensation; ++i) {
        //     bus_catch_up_cpu_cycles(1); 
        // }
        ////// the timing is not quite correct 
    }
    else if (addr == 0x4016) {
        controller_write(&controller1, value); 
    } 
    else if (addr == 0x4017) {
        controller_write(&controller2, value); 
    }
    else if (0x4000 <= addr && addr <= 0x4015) {
        // APU address 
    }
    else if (0x8000 <= addr) {
        panic("Error: trying to write to PRG ROM space! addr=%04x\n", addr); 
    }
    else {
        // printf("writing to address %04x is ignored\n", addr); 
    }
}

uint8_t read_prg_rom(uint16_t addr) {
    addr -= UINT16_C(0x8000);
    if ((bus.rom.prg_rom_size == 0x4000) && (addr >= 0x4000)) {
        addr %= 0x4000; 
    }
    return bus.rom.prg_rom[addr]; 
}

void bus_catch_up_cpu_cycles(unsigned cpu_cycles) {
    bus.cycles += cpu_cycles; 

    unsigned prev_nmi = ppu.nmi_raised; 
    ppu_tick_cycles(cpu_cycles * 3); 
    unsigned current_nmi = ppu.nmi_raised; 
    if (!prev_nmi && current_nmi) {
        // bus.nmi_callback(); 
        ppu_render_frame(); 
        controller_handle_input(0); 
        uint32_t tick = get_current_time_ms(); 
        int latency = 0; 
        if ((latency = (1000 / FRAMERATE) - (int)(tick - last_frame_tick)) > 0) {
            delay_ms(latency);
        }
        last_frame_tick = get_current_time_ms();
    }
        
}
bool bus_poll_nmi(void) {
    if (ppu.nmi_raised) {
        ppu.nmi_raised = false; 
        // bus.nmi_callback(); 
        return true; 
    } else {
        return false; 
    }
}







