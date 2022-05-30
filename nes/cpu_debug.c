#include "cpu_debug.h"

void dump_cpu(void) {
    uint8_t opcode = cpu_mem_read(cpu.pc); 
    uint16_t literal_address = cpu_mem_read(cpu.pc+1); 
    struct cpu_opcode op = opcodes[opcode]; 
    unsigned len = op.length; 
    printf("%04X  ", cpu.pc);
    uint8_t instr_hex[3] = {0, 0, 0}; 
    for (unsigned i = 0; i < 3; ++i) {
        if (i >= len) {
            printf("   "); 
        } else {
            instr_hex[i] = cpu_mem_read(cpu.pc + i); 
            printf("%02X ", instr_hex[i]); 
        }
    }
    if (op.name[0] == '*') {
        printf("%s ", op.name); 
    } else {
        printf(" %s ", op.name); 
    }
    
    uint16_t mem_addr = cpu_get_address(op.addrmode, cpu.pc + 1); 
    int cprinted = 0; 
    switch (op.addrmode) {
        case addressmode_imm: 
            cprinted += printf("#$%02X", literal_address); 
            break; 
        case addressmode_zeropage: 
            cprinted += printf("$%02X = %02X", mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_zeropage_x: 
            cprinted += printf("$%02X,X @ %02X = %02X", literal_address, mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_zeropage_y: 
            cprinted += printf("$%02X,Y @ %02X = %02X", literal_address, mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_absolute: 
            if (opcode == 0x4c || opcode == 0x20) {
                // JMP absolute or JSR absolute 
                cprinted += printf("$%04X", cpu_mem_read16(cpu.pc+1)); 
            } else {
                cprinted += printf("$%04X = %02X", mem_addr, cpu_mem_read(mem_addr));
            }
            break; 
        case addressmode_absolute_x: 
            literal_address = cpu_mem_read16(cpu.pc + 1); 
            cprinted += printf("$%04X,X @ %04X = %02X", literal_address, mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_absolute_y: 
            literal_address = cpu_mem_read16(cpu.pc + 1); 
            cprinted += printf("$%04X,Y @ %04X = %02X", literal_address, mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_indirect_x: 
            cprinted += printf("($%02X,X) @ %02X = %04X = %02X", literal_address, (literal_address+cpu.x)&0xFF, mem_addr, cpu_mem_read(mem_addr)); 
            break; 
        case addressmode_indirect_y: 
            cprinted += printf("($%02X),Y = %04X @ %04X = %02X", literal_address, (mem_addr-(uint16_t)cpu.y)&0xFFFF, mem_addr, cpu_mem_read(mem_addr));
            break;  
        case addressmode_none: 
            // cprinted += printf("                            ");
            if (opcode == 0x0a || opcode == 0x4a || opcode == 0x2a || opcode == 0x6a) {
                cprinted += printf("A "); 
            }
            break; 
        case addressmode_relative: 
            // branches 
            cprinted += printf("$%04X", ((int16_t)cpu.pc + 2 + (int16_t)((int8_t)literal_address)) & 0xFFFF);
            break; 
        case addressmode_indirect: 
            // jump indirect 
            {
                uint16_t jmp_addr = 0; 
                literal_address = cpu_mem_read16(cpu.pc + 1); 
                if ((literal_address & 0x00FF) == 0xFF) {
                    uint16_t lo = cpu_mem_read(literal_address); 
                    uint16_t hi = cpu_mem_read(literal_address & 0xFF00); 
                    jmp_addr = (hi << 8) | lo; 
                } else {
                    jmp_addr = cpu_mem_read16(literal_address); 
                }
                cprinted += printf("($%04X) = %04X", literal_address, jmp_addr); 
            }
            break; 
        default: 
            cprinted += printf("???");
            break; 
    }
    while (cprinted < 28) {
        cprinted += printf(" "); 
    }
    printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X", cpu.a, cpu.x, cpu.y, cpu.p, cpu.sp); 

    printf("\n"); 

}
void cpu_dump_state(void) {
    printf(
        "nes_cpu6502 = {\n"
        "    .a = %02x,\n"
        "    .x = %02x,\n"
        "    .y = %02x,\n"
        "    .p = %02x,\n"
        "    .pc = %04x,\n" 
        "    .sp = %02x,\n"
        "};\n", 
        cpu.a, 
        cpu.x, 
        cpu.y, 
        cpu.p, 
        cpu.pc, 
        cpu.sp 
    );
}











