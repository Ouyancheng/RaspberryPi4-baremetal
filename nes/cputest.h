#pragma once 
#include "cpu.h"

static void test_0xa9_lda_immidiate_load_data() {
    cpu_init(); 
    uint8_t program[] = {0xa9, 0x05, 0x00}; 
    cpu_load_program_and_run((uint8_t*)program, sizeof(program)); 
    assert(cpu.a == 5); 
    assert((cpu.p & UINT8_C(0b00000010)) == UINT8_C(0b00)); 
    assert((cpu.p & UINT8_C(0b10000000)) == UINT8_C(0)); 
}
static void test_0xaa_tax_move_a_to_x() {
    cpu_init(); 
    uint8_t program[] = {0xaa, 0x00}; 
    cpu_load_program((uint8_t*)program, sizeof(program));
    cpu_reset(); 
    cpu.a = 10;
    cpu_run(); 
    assert(cpu.x == 10); 
}
static void test_5_ops_working_together() {
    cpu_init(); 
    uint8_t program[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    cpu_load_program_and_run((uint8_t*)program, sizeof(program));
    assert(cpu.x == UINT8_C(0xc1)); 
}
static void test_inx_overflow() {
    cpu_init(); 
    uint8_t program[] = {0xe8, 0xe8, 0x00};
    cpu_load_program((uint8_t*)program, sizeof(program)); 
    cpu_reset(); 
    cpu.x = 0xff;
    cpu_run(); 
    assert(cpu.x == 1); 
}
static void test_lda_from_memory() {
    cpu_init(); 
    cpu_mem_write(0x10, 0x55); 
    uint8_t program[] = {0xa5, 0x10, 0x00};
    cpu_load_program_and_run((uint8_t*)program, sizeof(program)); 

    assert(cpu.a == 0x55);
}
static void test_opcode(void) {
    int invalid_count = 0; 
    int valid_count = 0; 
    for (unsigned i = 0; i < 0x100U; ++i) {
        if (opcodes[i].opcode != i || opcodes[i].length == 0) {
            printf("opcode %x is %x\n", i, opcodes[i].opcode); 
            // printf("[0x%02x] = {.name=\"???\", .length=0, .cycles=0, .addrmode=addressmode_none, .opcode=0x%02x},\n", i, i); 
            invalid_count += 1; 
        } else {
            valid_count += 1;
        }
    }
    printf("invalid_count = %d, valid_count = %d\n", invalid_count, valid_count); 
}
static void test_cpu(void) {
    test_opcode(); 
    test_0xa9_lda_immidiate_load_data(); 
    test_0xaa_tax_move_a_to_x(); 
    test_5_ops_working_together(); 
    test_inx_overflow(); 
    test_lda_from_memory(); 
    printf("all tests passed!\n"); 
}


















