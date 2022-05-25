#include "cpu_opcodes.h"
////// ported from https://github.com/bugzmanov/nes_ebook/blob/master/code/ch3.3/src/opcodes.rs 
struct cpu_opcode opcodes[0x100] = {
    [0x00] = {.name="BRK", .length=1, .cycles=7, .addrmode=addressmode_none, .opcode=0x00}, 
    [0xea] = {.name="NOP", .length=1, .cycles=2, .addrmode=addressmode_none, .opcode=0xea}, 
    /* Arithmetic */
    [0x69] = {"ADC", 2, 2, addressmode_imm, 0x69}, 
    [0x65] = {"ADC", 2, 3, addressmode_zeropage, 0x65},
    [0x75] = {"ADC", 2, 4, addressmode_zeropage_x, 0x75},
    [0x6d] = {"ADC", 3, 4, addressmode_absolute, 0x6d},
    [0x7d] = {"ADC", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0x7d},
    [0x79] = {"ADC", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0x79},
    [0x61] = {"ADC", 2, 6, addressmode_indirect_x, 0x61},
    [0x71] = {"ADC", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0x71},

    [0xe9] = {"SBC", 2, 2, addressmode_imm, 0xe9},
    [0xe5] = {"SBC", 2, 3, addressmode_zeropage, 0xe5},
    [0xf5] = {"SBC", 2, 4, addressmode_zeropage_x, 0xf5},
    [0xed] = {"SBC", 3, 4, addressmode_absolute, 0xed},
    [0xfd] = {"SBC", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0xfd},
    [0xf9] = {"SBC", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0xf9},
    [0xe1] = {"SBC", 2, 6, addressmode_indirect_x, 0xe1},
    [0xf1] = {"SBC", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0xf1},

    [0x29] = {"AND", 2, 2, addressmode_imm, 0x29},
    [0x25] = {"AND", 2, 3, addressmode_zeropage, 0x25},
    [0x35] = {"AND", 2, 4, addressmode_zeropage_x, 0x35},
    [0x2d] = {"AND", 3, 4, addressmode_absolute, 0x2d},
    [0x3d] = {"AND", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0x3d},
    [0x39] = {"AND", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0x39},
    [0x21] = {"AND", 2, 6, addressmode_indirect_x, 0x21},
    [0x31] = {"AND", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0x31},

    [0x49] = {"EOR", 2, 2, addressmode_imm, 0x49},
    [0x45] = {"EOR", 2, 3, addressmode_zeropage, 0x45},
    [0x55] = {"EOR", 2, 4, addressmode_zeropage_x, 0x55},
    [0x4d] = {"EOR", 3, 4, addressmode_absolute, 0x4d},
    [0x5d] = {"EOR", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0x5d},
    [0x59] = {"EOR", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0x59},
    [0x41] = {"EOR", 2, 6, addressmode_indirect_x, 0x41},
    [0x51] = {"EOR", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0x51},

    [0x09] = {"ORA", 2, 2, addressmode_imm, 0x09},
    [0x05] = {"ORA", 2, 3, addressmode_zeropage, 0x05},
    [0x15] = {"ORA", 2, 4, addressmode_zeropage_x, 0x15},
    [0x0d] = {"ORA", 3, 4, addressmode_absolute, 0x0d},
    [0x1d] = {"ORA", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0x1d},
    [0x19] = {"ORA", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0x19},
    [0x01] = {"ORA", 2, 6, addressmode_indirect_x, 0x01},
    [0x11] = {"ORA", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0x11},

    /* Shifts */
    [0x0a] = {"ASL", 1, 2, addressmode_none, 0x0a},
    [0x06] = {"ASL", 2, 5, addressmode_zeropage, 0x06},
    [0x16] = {"ASL", 2, 6, addressmode_zeropage_x, 0x16},
    [0x0e] = {"ASL", 3, 6, addressmode_absolute, 0x0e},
    [0x1e] = {"ASL", 3, 7, addressmode_absolute_x, 0x1e},

    [0x4a] = {"LSR", 1, 2, addressmode_none, 0x4a},
    [0x46] = {"LSR", 2, 5, addressmode_zeropage, 0x46},
    [0x56] = {"LSR", 2, 6, addressmode_zeropage_x, 0x56},
    [0x4e] = {"LSR", 3, 6, addressmode_absolute, 0x4e},
    [0x5e] = {"LSR", 3, 7, addressmode_absolute_x, 0x5e},

    [0x2a] = {"ROL", 1, 2, addressmode_none, 0x2a},
    [0x26] = {"ROL", 2, 5, addressmode_zeropage, 0x26},
    [0x36] = {"ROL", 2, 6, addressmode_zeropage_x, 0x36},
    [0x2e] = {"ROL", 3, 6, addressmode_absolute, 0x2e},
    [0x3e] = {"ROL", 3, 7, addressmode_absolute_x, 0x3e},

    [0x6a] = {"ROR", 1, 2, addressmode_none, 0x6a},
    [0x66] = {"ROR", 2, 5, addressmode_zeropage, 0x66},
    [0x76] = {"ROR", 2, 6, addressmode_zeropage_x, 0x76},
    [0x6e] = {"ROR", 3, 6, addressmode_absolute, 0x6e},
    [0x7e] = {"ROR", 3, 7, addressmode_absolute_x, 0x7e},

    [0xe6] = {"INC", 2, 5, addressmode_zeropage, 0xe6},
    [0xf6] = {"INC", 2, 6, addressmode_zeropage_x, 0xf6},
    [0xee] = {"INC", 3, 6, addressmode_absolute, 0xee},
    [0xfe] = {"INC", 3, 7, addressmode_absolute_x, 0xfe},

    [0xe8] = {"INX", 1, 2, addressmode_none, 0xe8},
    [0xc8] = {"INY", 1, 2, addressmode_none, 0xc8},

    [0xc6] = {"DEC", 2, 5, addressmode_zeropage, 0xc6},
    [0xd6] = {"DEC", 2, 6, addressmode_zeropage_x, 0xd6},
    [0xce] = {"DEC", 3, 6, addressmode_absolute, 0xce},
    [0xde] = {"DEC", 3, 7, addressmode_absolute_x, 0xde},

    [0xca] = {"DEX", 1, 2, addressmode_none, 0xca},
    [0x88] = {"DEY", 1, 2, addressmode_none, 0x88},

    [0xc9] = {"CMP", 2, 2, addressmode_imm, 0xc9},
    [0xc5] = {"CMP", 2, 3, addressmode_zeropage, 0xc5},
    [0xd5] = {"CMP", 2, 4, addressmode_zeropage_x, 0xd5},
    [0xcd] = {"CMP", 3, 4, addressmode_absolute, 0xcd},
    [0xdd] = {"CMP", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0xdd},
    [0xd9] = {"CMP", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0xd9},
    [0xc1] = {"CMP", 2, 6, addressmode_indirect_x, 0xc1},
    [0xd1] = {"CMP", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0xd1},

    [0xc0] = {"CPY", 2, 2, addressmode_imm, 0xc0},
    [0xc4] = {"CPY", 2, 3, addressmode_zeropage, 0xc4},
    [0xcc] = {"CPY", 3, 4, addressmode_absolute, 0xcc},

    [0xe0] = {"CPX", 2, 2, addressmode_imm, 0xe0},
    [0xe4] = {"CPX", 2, 3, addressmode_zeropage, 0xe4},
    [0xec] = {"CPX", 3, 4, addressmode_absolute, 0xec},


    /* Branching */

    [0x4c] = {"JMP", 3, 3, addressmode_none, 0x4c}, //AddressingMode that acts as Immidiate
    [0x6c] = {"JMP", 3, 5, addressmode_none, 0x6c}, //AddressingMode:Indirect with 6502 bug

    [0x20] = {"JSR", 3, 6, addressmode_none, 0x20},
    [0x60] = {"RTS", 1, 6, addressmode_none, 0x60},

    [0x40] = {"RTI", 1, 6, addressmode_none, 0x40},

    [0xd0] = {"BNE", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0xd0},
    [0x70] = {"BVS", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0x70},
    [0x50] = {"BVC", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0x50},
    [0x30] = {"BMI", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0x30},
    [0xf0] = {"BEQ", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0xf0},
    [0xb0] = {"BCS", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0xb0},
    [0x90] = {"BCC", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0x90},
    [0x10] = {"BPL", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_none, 0x10},

    [0x24] = {"BIT", 2, 3, addressmode_zeropage, 0x24},
    [0x2c] = {"BIT", 3, 4, addressmode_absolute, 0x2c},


    /* Stores, Loads */
    [0xa9] = {"LDA", 2, 2, addressmode_imm, 0xa9},
    [0xa5] = {"LDA", 2, 3, addressmode_zeropage, 0xa5},
    [0xb5] = {"LDA", 2, 4, addressmode_zeropage_x, 0xb5},
    [0xad] = {"LDA", 3, 4, addressmode_absolute, 0xad},
    [0xbd] = {"LDA", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0xbd},
    [0xb9] = {"LDA", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0xb9},
    [0xa1] = {"LDA", 2, 6, addressmode_indirect_x, 0xa1},
    [0xb1] = {"LDA", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, 0xb1},

    [0xa2] = {"LDX", 2, 2, addressmode_imm, 0xa2},
    [0xa6] = {"LDX", 2, 3, addressmode_zeropage, 0xa6},
    [0xb6] = {"LDX", 2, 4, addressmode_zeropage_y, 0xb6},
    [0xae] = {"LDX", 3, 4, addressmode_absolute, 0xae},
    [0xbe] = {"LDX", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, 0xbe},

    [0xa0] = {"LDY", 2, 2, addressmode_imm, 0xa0},
    [0xa4] = {"LDY", 2, 3, addressmode_zeropage, 0xa4},
    [0xb4] = {"LDY", 2, 4, addressmode_zeropage_x, 0xb4},
    [0xac] = {"LDY", 3, 4, addressmode_absolute, 0xac},
    [0xbc] = {"LDY", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, 0xbc},


    [0x85] = {"STA", 2, 3, addressmode_zeropage, 0x85},
    [0x95] = {"STA", 2, 4, addressmode_zeropage_x, 0x95},
    [0x8d] = {"STA", 3, 4, addressmode_absolute, 0x8d},
    [0x9d] = {"STA", 3, 5, addressmode_absolute_x, 0x9d},
    [0x99] = {"STA", 3, 5, addressmode_absolute_y, 0x99},
    [0x81] = {"STA", 2, 6, addressmode_indirect_x, 0x81},
    [0x91] = {"STA", 2, 6, addressmode_indirect_y, 0x91},

    [0x86] = {"STX", 2, 3, addressmode_zeropage, 0x86},
    [0x96] = {"STX", 2, 4, addressmode_zeropage_y, 0x96},
    [0x8e] = {"STX", 3, 4, addressmode_absolute, 0x8e},

    [0x84] = {"STY", 2, 3, addressmode_zeropage, 0x84},
    [0x94] = {"STY", 2, 4, addressmode_zeropage_x, 0x94},
    [0x8c] = {"STY", 3, 4, addressmode_absolute, 0x8c},


    /* Flags clear */

    [0xd8] = {"CLD", 1, 2, addressmode_none, 0xd8},
    [0x58] = {"CLI", 1, 2, addressmode_none, 0x58},
    [0xb8] = {"CLV", 1, 2, addressmode_none, 0xb8},
    [0x18] = {"CLC", 1, 2, addressmode_none, 0x18},
    [0x38] = {"SEC", 1, 2, addressmode_none, 0x38},
    [0x78] = {"SEI", 1, 2, addressmode_none, 0x78},
    [0xf8] = {"SED", 1, 2, addressmode_none, 0xf8},

    [0xaa] = {"TAX", 1, 2, addressmode_none, 0xaa},
    [0xa8] = {"TAY", 1, 2, addressmode_none, 0xa8},
    [0xba] = {"TSX", 1, 2, addressmode_none, 0xba},
    [0x8a] = {"TXA", 1, 2, addressmode_none, 0x8a},
    [0x9a] = {"TXS", 1, 2, addressmode_none, 0x9a},
    [0x98] = {"TYA", 1, 2, addressmode_none, 0x98},

    /* Stack */
    [0x48] = {"PHA", 1, 3, addressmode_none, 0x48},
    [0x68] = {"PLA", 1, 4, addressmode_none, 0x68},
    [0x08] = {"PHP", 1, 3, addressmode_none, 0x08},
    [0x28] = {"PLP", 1, 4, addressmode_none, 0x28},


    /* Unknown */
    [0x02] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x02},
    [0x03] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x03},
    [0x04] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x04},
    [0x07] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x07},
    [0x0b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x0b},
    [0x0c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x0c},
    [0x0f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x0f},
    [0x12] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x12},
    [0x13] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x13},
    [0x14] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x14},
    [0x17] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x17},
    [0x1a] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x1a},
    [0x1b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x1b},
    [0x1c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x1c},
    [0x1f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x1f},
    [0x22] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x22},
    [0x23] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x23},
    [0x27] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x27},
    [0x2b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x2b},
    [0x2f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x2f},
    [0x32] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x32},
    [0x33] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x33},
    [0x34] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x34},
    [0x37] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x37},
    [0x3a] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x3a},
    [0x3b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x3b},
    [0x3c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x3c},
    [0x3f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x3f},
    [0x42] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x42},
    [0x43] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x43},
    [0x44] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x44},
    [0x47] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x47},
    [0x4b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x4b},
    [0x4f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x4f},
    [0x52] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x52},
    [0x53] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x53},
    [0x54] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x54},
    [0x57] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x57},
    [0x5a] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x5a},
    [0x5b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x5b},
    [0x5c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x5c},
    [0x5f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x5f},
    [0x62] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x62},
    [0x63] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x63},
    [0x64] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x64},
    [0x67] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x67},
    [0x6b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x6b},
    [0x6f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x6f},
    [0x72] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x72},
    [0x73] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x73},
    [0x74] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x74},
    [0x77] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x77},
    [0x7a] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x7a},
    [0x7b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x7b},
    [0x7c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x7c},
    [0x7f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x7f},
    [0x80] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x80},
    [0x82] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x82},
    [0x83] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x83},
    [0x87] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x87},
    [0x89] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x89},
    [0x8b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x8b},
    [0x8f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x8f},
    [0x92] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x92},
    [0x93] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x93},
    [0x97] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x97},
    [0x9b] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x9b},
    [0x9c] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x9c},
    [0x9e] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x9e},
    [0x9f] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x9f},
    [0xa3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xa3},
    [0xa7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xa7},
    [0xab] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xab},
    [0xaf] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xaf},
    [0xb2] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xb2},
    [0xb3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xb3},
    [0xb7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xb7},
    [0xbb] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xbb},
    [0xbf] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xbf},
    [0xc2] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xc2},
    [0xc3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xc3},
    [0xc7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xc7},
    [0xcb] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xcb},
    [0xcf] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xcf},
    [0xd2] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xd2},
    [0xd3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xd3},
    [0xd4] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xd4},
    [0xd7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xd7},
    [0xda] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xda},
    [0xdb] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xdb},
    [0xdc] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xdc},
    [0xdf] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xdf},
    [0xe2] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xe2},
    [0xe3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xe3},
    [0xe7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xe7},
    [0xeb] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xeb},
    [0xef] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xef},
    [0xf2] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xf2},
    [0xf3] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xf3},
    [0xf4] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xf4},
    [0xf7] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xf7},
    [0xfa] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xfa},
    [0xfb] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xfb},
    [0xfc] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xfc},
    [0xff] = {.name="???", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0xff},
};