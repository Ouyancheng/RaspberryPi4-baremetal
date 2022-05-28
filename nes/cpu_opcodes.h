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
enum cpu_instruction {
    INVALID, 
    LDA,
    TAX,
    INX,
    BRK,
    CLD,
    CLI,
    CLV, 
    CLC, 
    SEC, 
    SEI,
    SED,
    PHA,
    PLA,
    PHP,
    PLP,
    ADC,
    SBC,
    AND,
    EOR,
    ORA,
    LSR_A,
    LSR,
    ASL_A,
    ASL, 
    ROL_A,
    ROL,
    ROR_A,
    ROR,
    INC,
    INY,
    DEC,
    DEX,
    DEY,
    CMP,
    CPY,
    CPX,
    JMP_ABS, 
    JMP,
    JSR,
    RTS,
    RTI,
    BNE,
    BVS,
    BVC,
    BPL,
    BMI,
    BEQ,
    BCS,
    BCC,
    BIT,
    STA,
    STX,
    STY,
    LDX,
    LDY,
    NOP,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
    ////// unofficial ////// 
    _DCP, 
    _RLA,
    _SLO,
    _SRE,
    _SKB,
    _AXS,
    _ARR, 
    _SBC, 
    _ANC, 
    _ALR, 
    _NOP_R,
    _RRA, 
    _ISB, 
    _NOP, 
    _LAX, 
    _SAX, 
    _LXA, 
    _XAA, // the highly unstable one? 
    _LAS, 
    _TAS, 
    _AHX_IND_Y, 
    _AHX_ABS_Y, 
    _SHX, 
    _SHY, 
};
struct cpu_opcode {
    char name[5]; 
    unsigned length;  
    unsigned cycles;
    enum addressmode addrmode; 
    enum cpu_instruction instruction; 
};
extern struct cpu_opcode opcodes[0x100]; 





