#pragma once 

#include "sdk.h"


struct nes_controller {
    // write 1 to start filling in the shift register, while only returning the first button, write 0 to stop filling 
    bool strobe;  
    int latched_index; 
    uint8_t shift_register; 
};
enum controller_button {
    controller_button_right    = 0b10000000,
    controller_button_left     = 0b01000000,
    controller_button_down     = 0b00100000,
    controller_button_up       = 0b00010000,
    controller_button_start    = 0b00001000,
    controller_button_select   = 0b00000100,
    controller_button_button_b = 0b00000010,
    controller_button_button_a = 0b00000001,
};

void controller_init(struct nes_controller *ctl); 
void controller_write(struct nes_controller *ctl, uint8_t data); 
uint8_t controller_read(struct nes_controller *ctl); 
void controller_latch_button(struct nes_controller *ctl, enum controller_button button, bool pressed); 

extern struct nes_controller controller1; 
extern struct nes_controller controller2; 


















