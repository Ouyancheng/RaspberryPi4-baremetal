#include "cpu_opcodes.h"
////// ported from https://github.com/bugzmanov/nes_ebook/blob/master/code/ch3.3/src/opcodes.rs 
struct cpu_opcode opcodes[0x100] = {
    [0x00] = {.name="BRK", .length=1, .cycles=7, .addrmode=addressmode_none, .instruction=BRK}, 
    [0xea] = {.name="NOP", .length=1, .cycles=2, .addrmode=addressmode_none, .instruction=NOP}, 
    /* Arithmetic */
    [0x69] = {"ADC", 2, 2, addressmode_imm, ADC}, 
    [0x65] = {"ADC", 2, 3, addressmode_zeropage, ADC},
    [0x75] = {"ADC", 2, 4, addressmode_zeropage_x, ADC},
    [0x6d] = {"ADC", 3, 4, addressmode_absolute, ADC},
    [0x7d] = {"ADC", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, ADC},
    [0x79] = {"ADC", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, ADC},
    [0x61] = {"ADC", 2, 6, addressmode_indirect_x, ADC},
    [0x71] = {"ADC", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, ADC},

    [0xe9] = {"SBC", 2, 2, addressmode_imm, SBC},
    [0xe5] = {"SBC", 2, 3, addressmode_zeropage, SBC},
    [0xf5] = {"SBC", 2, 4, addressmode_zeropage_x, SBC},
    [0xed] = {"SBC", 3, 4, addressmode_absolute, SBC},
    [0xfd] = {"SBC", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, SBC},
    [0xf9] = {"SBC", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, SBC},
    [0xe1] = {"SBC", 2, 6, addressmode_indirect_x, SBC},
    [0xf1] = {"SBC", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, SBC},

    [0x29] = {"AND", 2, 2, addressmode_imm, AND},
    [0x25] = {"AND", 2, 3, addressmode_zeropage, AND},
    [0x35] = {"AND", 2, 4, addressmode_zeropage_x, AND},
    [0x2d] = {"AND", 3, 4, addressmode_absolute, AND},
    [0x3d] = {"AND", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, AND},
    [0x39] = {"AND", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, AND},
    [0x21] = {"AND", 2, 6, addressmode_indirect_x, AND},
    [0x31] = {"AND", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, AND},

    [0x49] = {"EOR", 2, 2, addressmode_imm, EOR},
    [0x45] = {"EOR", 2, 3, addressmode_zeropage, EOR},
    [0x55] = {"EOR", 2, 4, addressmode_zeropage_x, EOR},
    [0x4d] = {"EOR", 3, 4, addressmode_absolute, EOR},
    [0x5d] = {"EOR", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, EOR},
    [0x59] = {"EOR", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, EOR},
    [0x41] = {"EOR", 2, 6, addressmode_indirect_x, EOR},
    [0x51] = {"EOR", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, EOR},

    [0x09] = {"ORA", 2, 2, addressmode_imm, ORA},
    [0x05] = {"ORA", 2, 3, addressmode_zeropage, ORA},
    [0x15] = {"ORA", 2, 4, addressmode_zeropage_x, ORA},
    [0x0d] = {"ORA", 3, 4, addressmode_absolute, ORA},
    [0x1d] = {"ORA", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, ORA},
    [0x19] = {"ORA", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, ORA},
    [0x01] = {"ORA", 2, 6, addressmode_indirect_x, ORA},
    [0x11] = {"ORA", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, ORA},

    /* Shifts */
    [0x0a] = {"ASL", 1, 2, addressmode_none, ASL_A},
    [0x06] = {"ASL", 2, 5, addressmode_zeropage, ASL},
    [0x16] = {"ASL", 2, 6, addressmode_zeropage_x, ASL},
    [0x0e] = {"ASL", 3, 6, addressmode_absolute, ASL},
    [0x1e] = {"ASL", 3, 7, addressmode_absolute_x, ASL},

    [0x4a] = {"LSR", 1, 2, addressmode_none, LSR_A},
    [0x46] = {"LSR", 2, 5, addressmode_zeropage, LSR},
    [0x56] = {"LSR", 2, 6, addressmode_zeropage_x, LSR},
    [0x4e] = {"LSR", 3, 6, addressmode_absolute, LSR},
    [0x5e] = {"LSR", 3, 7, addressmode_absolute_x, LSR},

    [0x2a] = {"ROL", 1, 2, addressmode_none, ROL_A},
    [0x26] = {"ROL", 2, 5, addressmode_zeropage, ROL},
    [0x36] = {"ROL", 2, 6, addressmode_zeropage_x, ROL},
    [0x2e] = {"ROL", 3, 6, addressmode_absolute, ROL},
    [0x3e] = {"ROL", 3, 7, addressmode_absolute_x, ROL},

    [0x6a] = {"ROR", 1, 2, addressmode_none, ROR_A},
    [0x66] = {"ROR", 2, 5, addressmode_zeropage, ROR},
    [0x76] = {"ROR", 2, 6, addressmode_zeropage_x, ROR},
    [0x6e] = {"ROR", 3, 6, addressmode_absolute, ROR},
    [0x7e] = {"ROR", 3, 7, addressmode_absolute_x, ROR},

    [0xe6] = {"INC", 2, 5, addressmode_zeropage, INC},
    [0xf6] = {"INC", 2, 6, addressmode_zeropage_x, INC},
    [0xee] = {"INC", 3, 6, addressmode_absolute, INC},
    [0xfe] = {"INC", 3, 7, addressmode_absolute_x, INC},

    [0xe8] = {"INX", 1, 2, addressmode_none, INX},
    [0xc8] = {"INY", 1, 2, addressmode_none, INY},

    [0xc6] = {"DEC", 2, 5, addressmode_zeropage, DEC},
    [0xd6] = {"DEC", 2, 6, addressmode_zeropage_x, DEC},
    [0xce] = {"DEC", 3, 6, addressmode_absolute, DEC},
    [0xde] = {"DEC", 3, 7, addressmode_absolute_x, DEC},

    [0xca] = {"DEX", 1, 2, addressmode_none, DEX},
    [0x88] = {"DEY", 1, 2, addressmode_none, DEY},

    [0xc9] = {"CMP", 2, 2, addressmode_imm, CMP},
    [0xc5] = {"CMP", 2, 3, addressmode_zeropage, CMP},
    [0xd5] = {"CMP", 2, 4, addressmode_zeropage_x, CMP},
    [0xcd] = {"CMP", 3, 4, addressmode_absolute, CMP},
    [0xdd] = {"CMP", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, CMP},
    [0xd9] = {"CMP", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, CMP},
    [0xc1] = {"CMP", 2, 6, addressmode_indirect_x, CMP},
    [0xd1] = {"CMP", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, CMP},

    [0xc0] = {"CPY", 2, 2, addressmode_imm, CPY},
    [0xc4] = {"CPY", 2, 3, addressmode_zeropage, CPY},
    [0xcc] = {"CPY", 3, 4, addressmode_absolute, CPY},

    [0xe0] = {"CPX", 2, 2, addressmode_imm, CPX},
    [0xe4] = {"CPX", 2, 3, addressmode_zeropage, CPX},
    [0xec] = {"CPX", 3, 4, addressmode_absolute, CPX},


    /* Branching */

    [0x4c] = {"JMP", 3, 3, addressmode_absolute, JMP_ABS}, 
    [0x6c] = {"JMP", 3, 5, addressmode_indirect, JMP}, // 6502 has bug on this one 

    [0x20] = {"JSR", 3, 6, addressmode_absolute, JSR},
    [0x60] = {"RTS", 1, 6, addressmode_none, RTS},

    [0x40] = {"RTI", 1, 6, addressmode_none, RTI},

    [0xd0] = {"BNE", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BNE},
    [0x70] = {"BVS", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BVS},
    [0x50] = {"BVC", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BVC},
    [0x30] = {"BMI", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BMI},
    [0xf0] = {"BEQ", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BEQ},
    [0xb0] = {"BCS", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BCS},
    [0x90] = {"BCC", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BCC},
    [0x10] = {"BPL", 2, 2 /*(+1 if branch succeeds +2 if to a new page)*/, addressmode_relative, BPL},
    

    [0x24] = {"BIT", 2, 3, addressmode_zeropage, BIT},
    [0x2c] = {"BIT", 3, 4, addressmode_absolute, BIT},


    /* Stores, Loads */
    [0xa9] = {"LDA", 2, 2, addressmode_imm, LDA},
    [0xa5] = {"LDA", 2, 3, addressmode_zeropage, LDA},
    [0xb5] = {"LDA", 2, 4, addressmode_zeropage_x, LDA},
    [0xad] = {"LDA", 3, 4, addressmode_absolute, LDA},
    [0xbd] = {"LDA", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, LDA},
    [0xb9] = {"LDA", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, LDA},
    [0xa1] = {"LDA", 2, 6, addressmode_indirect_x, LDA},
    [0xb1] = {"LDA", 2, 5/*+1 if page crossed*/, addressmode_indirect_y, LDA},

    [0xa2] = {"LDX", 2, 2, addressmode_imm, LDX},
    [0xa6] = {"LDX", 2, 3, addressmode_zeropage, LDX},
    [0xb6] = {"LDX", 2, 4, addressmode_zeropage_y, LDX},
    [0xae] = {"LDX", 3, 4, addressmode_absolute, LDX},
    [0xbe] = {"LDX", 3, 4/*+1 if page crossed*/, addressmode_absolute_y, LDX},

    [0xa0] = {"LDY", 2, 2, addressmode_imm, LDY},
    [0xa4] = {"LDY", 2, 3, addressmode_zeropage, LDY},
    [0xb4] = {"LDY", 2, 4, addressmode_zeropage_x, LDY},
    [0xac] = {"LDY", 3, 4, addressmode_absolute, LDY},
    [0xbc] = {"LDY", 3, 4/*+1 if page crossed*/, addressmode_absolute_x, LDY},


    [0x85] = {"STA", 2, 3, addressmode_zeropage, STA},
    [0x95] = {"STA", 2, 4, addressmode_zeropage_x, STA},
    [0x8d] = {"STA", 3, 4, addressmode_absolute, STA},
    [0x9d] = {"STA", 3, 5, addressmode_absolute_x, STA},
    [0x99] = {"STA", 3, 5, addressmode_absolute_y, STA},
    [0x81] = {"STA", 2, 6, addressmode_indirect_x, STA},
    [0x91] = {"STA", 2, 6, addressmode_indirect_y, STA},

    [0x86] = {"STX", 2, 3, addressmode_zeropage, STX},
    [0x96] = {"STX", 2, 4, addressmode_zeropage_y, STX},
    [0x8e] = {"STX", 3, 4, addressmode_absolute, STX},

    [0x84] = {"STY", 2, 3, addressmode_zeropage, STY},
    [0x94] = {"STY", 2, 4, addressmode_zeropage_x, STY},
    [0x8c] = {"STY", 3, 4, addressmode_absolute, STY},


    /* Flags clear */

    [0xd8] = {"CLD", 1, 2, addressmode_none, CLD},
    [0x58] = {"CLI", 1, 2, addressmode_none, CLI},
    [0xb8] = {"CLV", 1, 2, addressmode_none, CLV},
    [0x18] = {"CLC", 1, 2, addressmode_none, CLC},
    [0x38] = {"SEC", 1, 2, addressmode_none, SEC},
    [0x78] = {"SEI", 1, 2, addressmode_none, SEI},
    [0xf8] = {"SED", 1, 2, addressmode_none, SED},

    [0xaa] = {"TAX", 1, 2, addressmode_none, TAX},
    [0xa8] = {"TAY", 1, 2, addressmode_none, TAY},
    [0xba] = {"TSX", 1, 2, addressmode_none, TSX},
    [0x8a] = {"TXA", 1, 2, addressmode_none, TXA},
    [0x9a] = {"TXS", 1, 2, addressmode_none, TXS},
    [0x98] = {"TYA", 1, 2, addressmode_none, TYA},

    /* Stack */
    [0x48] = {"PHA", 1, 3, addressmode_none, PHA},
    [0x68] = {"PLA", 1, 4, addressmode_none, PLA},
    [0x08] = {"PHP", 1, 3, addressmode_none, PHP},
    [0x28] = {"PLP", 1, 4, addressmode_none, PLP},


    /* Unofficial */
    /// Source: https://github.com/bugzmanov/nes_ebook/blob/master/code/ch5.1/src/opcodes.rs 
    /* DCP family */ 
    [0xc7] = {.name="*DCP", .length=2, .cycles=5, .addrmode=addressmode_zeropage, .instruction=_DCP},
    [0xd7] = {.name="*DCP", .length=2, .cycles=6, .addrmode=addressmode_zeropage_x, .instruction=_DCP},
    [0xcf] = {.name="*DCP", .length=3, .cycles=6, .addrmode=addressmode_absolute, .instruction=_DCP},
    [0xdf] = {.name="*DCP", .length=3, .cycles=7, .addrmode=addressmode_absolute_x, .instruction=_DCP},
    [0xdb] = {.name="*DCP", .length=3, .cycles=7, .addrmode=addressmode_absolute_y, .instruction=_DCP},
    [0xc3] = {.name="*DCP", .length=2, .cycles=8, .addrmode=addressmode_indirect_x, .instruction=_DCP},
    [0xd3] = {.name="*DCP", .length=2, .cycles=8, .addrmode=addressmode_indirect_y, .instruction=_DCP},

    /* RLA family */
    [0x27] = {"*RLA", 2, 5, addressmode_zeropage, _RLA},
    [0x37] = {"*RLA", 2, 6, addressmode_zeropage_x, _RLA},
    [0x2F] = {"*RLA", 3, 6, addressmode_absolute, _RLA},
    [0x3F] = {"*RLA", 3, 7, addressmode_absolute_x, _RLA},
    [0x3b] = {"*RLA", 3, 7, addressmode_absolute_y, _RLA},
    [0x33] = {"*RLA", 2, 8, addressmode_indirect_y, _RLA},
    [0x23] = {"*RLA", 2, 8, addressmode_indirect_x, _RLA},

    [0x07] = {"*SLO", 2, 5, addressmode_zeropage, _SLO},
    [0x17] = {"*SLO", 2, 6, addressmode_zeropage_x, _SLO},
    [0x0F] = {"*SLO", 3, 6, addressmode_absolute, _SLO},
    [0x1f] = {"*SLO", 3, 7, addressmode_absolute_x, _SLO},
    [0x1b] = {"*SLO", 3, 7, addressmode_absolute_y, _SLO},
    [0x03] = {"*SLO", 2, 8, addressmode_indirect_x, _SLO},
    [0x13] = {"*SLO", 2, 8, addressmode_indirect_y, _SLO},

    [0x47] = {"*SRE", 2, 5, addressmode_zeropage, _SRE},
    [0x57] = {"*SRE", 2, 6, addressmode_zeropage_x, _SRE},
    [0x4F] = {"*SRE", 3, 6, addressmode_absolute, _SRE},
    [0x5f] = {"*SRE", 3, 7, addressmode_absolute_x, _SRE},
    [0x5b] = {"*SRE", 3, 7, addressmode_absolute_y, _SRE},
    [0x43] = {"*SRE", 2, 8, addressmode_indirect_x, _SRE},
    [0x53] = {"*SRE", 2, 8, addressmode_indirect_y, _SRE},


    [0x80] = {"*NOP", 2,2, addressmode_imm, _SKB},
    [0x82] = {"*NOP", 2,2, addressmode_imm, _SKB},
    [0x89] = {"*NOP", 2,2, addressmode_imm, _SKB},
    [0xc2] = {"*NOP", 2,2, addressmode_imm, _SKB},
    [0xe2] = {"*NOP", 2,2, addressmode_imm, _SKB},

    [0xCB] = {"*AXS", 2,2, addressmode_imm, _AXS},

    [0x6B] = {"*ARR", 2,2, addressmode_imm, _ARR},

    [0xeb] = {"*SBC", 2,2, addressmode_imm, _SBC},

    [0x0b] = {"*ANC", 2,2, addressmode_imm, _ANC},
    [0x2b] = {"*ANC", 2,2, addressmode_imm, _ANC},

    [0x4b] = {"*ALR", 2,2, addressmode_imm, _ALR},

    [0x04] = {"*NOP", 2,3, addressmode_zeropage, _NOP_R},
    [0x44] = {"*NOP", 2,3, addressmode_zeropage, _NOP_R},
    [0x64] = {"*NOP", 2,3, addressmode_zeropage, _NOP_R},
    [0x14] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0x34] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0x54] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0x74] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0xd4] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0xf4] = {"*NOP", 2, 4, addressmode_zeropage_x, _NOP_R},
    [0x0c] = {"*NOP", 3, 4, addressmode_absolute, _NOP_R},
    [0x1c] = {"*NOP", 3, 4 /*or 5*/, addressmode_absolute_x, _NOP_R},
    [0x3c] = {"*NOP", 3, 4 /*or 5*/, addressmode_absolute_x, _NOP_R},
    [0x5c] = {"*NOP", 3, 4 /*or 5*/, addressmode_absolute_x, _NOP_R},
    [0x7c] = {"*NOP", 3, 4 /*or 5*/, addressmode_absolute_x, _NOP_R},
    [0xdc] = {"*NOP", 3, 4 /* or 5*/, addressmode_absolute_x, _NOP_R},
    [0xfc] = {"*NOP", 3, 4 /* or 5*/, addressmode_absolute_x, _NOP_R},

    [0x67] = {"*RRA", 2, 5, addressmode_zeropage, _RRA},
    [0x77] = {"*RRA", 2, 6, addressmode_zeropage_x, _RRA},
    [0x6f] = {"*RRA", 3, 6, addressmode_absolute, _RRA},
    [0x7f] = {"*RRA", 3, 7, addressmode_absolute_x, _RRA},
    [0x7b] = {"*RRA", 3, 7, addressmode_absolute_y, _RRA},
    [0x63] = {"*RRA", 2, 8, addressmode_indirect_x, _RRA},
    [0x73] = {"*RRA", 2, 8, addressmode_indirect_y, _RRA},


    [0xe7] = {"*ISB", 2,5, addressmode_zeropage, _ISB},
    [0xf7] = {"*ISB", 2,6, addressmode_zeropage_x, _ISB},
    [0xef] = {"*ISB", 3,6, addressmode_absolute, _ISB},
    [0xff] = {"*ISB", 3,7, addressmode_absolute_x, _ISB},
    [0xfb] = {"*ISB", 3,7, addressmode_absolute_y, _ISB},
    [0xe3] = {"*ISB", 2,8, addressmode_indirect_x, _ISB},
    [0xf3] = {"*ISB", 2,8, addressmode_indirect_y, _ISB},

    [0x02] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x12] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x22] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x32] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x42] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x52] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x62] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x72] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x92] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0xb2] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0xd2] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0xf2] = {"*NOP", 1,2, addressmode_none, _NOP},

    [0x1a] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x3a] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x5a] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0x7a] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0xda] = {"*NOP", 1,2, addressmode_none, _NOP},
    [0xfa] = {"*NOP", 1,2, addressmode_none, _NOP},

    // highly unstable and not used
    [0xab] = {"*LXA", 2, 3, addressmode_imm, _LXA}, 
    [0x8b] = {"*XAA", 2, 3, addressmode_imm, _XAA}, 
    [0xbb] = {"*LAS", 3, 2, addressmode_absolute_y, _LAS}, 
    [0x9b] = {"*TAS", 3, 2, addressmode_absolute_y, _TAS}, 
    [0x93] = {"*AHX", 2, /* guess */ 8, addressmode_indirect_y, _AHX_IND_Y}, 
    [0x9f] = {"*AHX", 3, /* guess */ 4/* or 5*/, addressmode_absolute_y, _AHX_ABS_Y}, 
    [0x9e] = {"*SHX", 3, /* guess */ 4/* or 5*/, addressmode_absolute_y, _SHX}, 
    [0x9c] = {"*SHY", 3, /* guess */ 4/* or 5*/, addressmode_absolute_x, _SHY}, 

    [0xa7] = {"*LAX", 2, 3, addressmode_zeropage, _LAX},
    [0xb7] = {"*LAX", 2, 4, addressmode_zeropage_y, _LAX},
    [0xaf] = {"*LAX", 3, 4, addressmode_absolute, _LAX},
    [0xbf] = {"*LAX", 3, 4, addressmode_absolute_y, _LAX},
    [0xa3] = {"*LAX", 2, 6, addressmode_indirect_x, _LAX},
    [0xb3] = {"*LAX", 2, 5, addressmode_indirect_y, _LAX},

    [0x87] = {"*SAX", 2, 3, addressmode_zeropage, _SAX},
    [0x97] = {"*SAX", 2, 4, addressmode_zeropage_y, _SAX},
    [0x8f] = {"*SAX", 3, 4, addressmode_absolute, _SAX},
    [0x83] = {"*SAX", 2, 6, addressmode_indirect_x, _SAX},
};

