#pragma once 
#include "sdk.h"
enum addressmode {
    addressmode_imm, 
    addressmode_zeropage, 
    addressmode_zeropage_x, 
    addressmode_zeropage_y, 
    addressmode_absolute, 
    addressmode_absolute_x, 
    addressmode_absolute_y,
    addressmode_indirect_x, 
    addressmode_indirect_y, 

    addressmode_relative, 
    addressmode_indirect, 
    addressmode_none 
};
struct cpu_opcode {
    char name[4]; 
    unsigned length;  
    unsigned cycles;
    enum addressmode addrmode; 
    uint8_t opcode; 
};
extern struct cpu_opcode opcodes[0x100]; 
