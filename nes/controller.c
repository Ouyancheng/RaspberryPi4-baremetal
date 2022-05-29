#include "controller.h"

void controller_init(struct nes_controller *ctl) {
    ctl->strobe = 0; 
    ctl->latched_index = 0; 
    ctl->shift_register = 0; 
}

void controller_write(struct nes_controller *ctl, uint8_t data) {
    ctl->strobe = (data & 1);
    if (ctl->strobe & 1) {
        // ctl->shift_register = 0; 
        ctl->latched_index = 0; 
    }
}
uint8_t controller_read(struct nes_controller *ctl) {
    if (ctl->strobe) {
        return (ctl->shift_register & 1); 
    }
    // After 8 bits are read, 
    // all subsequent bits will report 1 on a standard NES controller, 
    // but third party and other controllers may report other values here.
    if (ctl->latched_index >= 8) {
        return 1; 
    }
    uint8_t bit = ((ctl->shift_register >> ctl->latched_index) & 1); 
    #if PLATFORM_RPI
    ctl->shift_register &= ~(UINT8_C(1) << ctl->latched_index); 
    #endif 
    ctl->latched_index += 1; 
    return bit; 
}
void controller_latch_button(struct nes_controller *ctl, enum controller_button button, bool pressed) {
    if (pressed) ctl->shift_register |= button; 
    else ctl->shift_register &= ~((uint8_t)button); 
}



struct nes_controller controller1; 

struct nes_controller controller2; 

