#include "cpu_exec.h"
#include "cpu.h"
#if INLINE_OP
#   define INLINE static inline
#else 
#   define INLINE static 
#endif 

static inline bool page_crossed(uint16_t addr1, uint16_t addr2) {
    return (addr1 & UINT16_C(0xFF00)) != (addr2 & UINT16_C(0xFF00)); 
}

/// get the address from addressing mode, NOTE: pc is pointing at the next byte 
uint16_t get_address_vaddr(enum addressmode mode, uint16_t addr) {
    cpu.page_crossed = false; 
    switch (mode) {
    case addressmode_imm:
        return addr;
    case addressmode_zeropage: 
        return (uint16_t)cpu_mem_read(addr); 
    case addressmode_absolute: 
        return cpu_mem_read16(addr); 
    case addressmode_zeropage_x: 
        return (uint16_t)((cpu.x + cpu_mem_read(addr)) & 0xFF); 
    case addressmode_zeropage_y: 
        return (uint16_t)((cpu.y + cpu_mem_read(addr)) & 0xFF); 
    case addressmode_absolute_x: {
        uint16_t base = cpu_mem_read16(addr);
        uint16_t target = base + (uint16_t)cpu.x; 
        cpu.page_crossed = page_crossed(base, target); 
        return target; 
    }
    case addressmode_absolute_y: {
        uint16_t base = cpu_mem_read16(addr); 
        uint16_t target = base + (uint16_t)cpu.y; 
        cpu.page_crossed = page_crossed(base, target); 
        return target; 
    }
    case addressmode_indirect_x: {
        uint8_t base = cpu_mem_read(addr); 
        uint8_t ptr = (base + cpu.x) & 0xff; 
        uint8_t lo = cpu_mem_read(ptr); 
        uint8_t hi = cpu_mem_read((ptr+1) & 0xff); 
        return (((uint16_t)hi) << 8) | (uint16_t)lo; 
    }
    case addressmode_indirect_y: {
        uint8_t base = cpu_mem_read(addr); 
        uint8_t lo = cpu_mem_read(base); 
        uint8_t hi = cpu_mem_read((base+1) & 0xff); 
        uint16_t base2 = ((((uint16_t)hi) << 8) | (uint16_t)lo);
        uint16_t target = base2 + (uint16_t)cpu.y; 
        cpu.page_crossed = page_crossed(base2, target); 
        return target; 
    }
    case addressmode_indirect: {
        uint16_t base = cpu_mem_read16(addr); 
        return cpu_mem_read16(base); 
    }
    case addressmode_relative: {
        ////// may be correct 
        uint8_t offset = cpu_mem_read(addr); 
        return (addr + 1 + (uint16_t)offset) & 0xffff; 
    }
    case addressmode_none: 
        // printf("Error: get_address in none mode\n"); 
        return 0; 
    default: 
        printf("Error: get_address invalid mode %d\n", (int)mode); 
        break;
    }
    return 0; 
}
static inline uint16_t get_address(enum addressmode mode) {
    return get_address_vaddr(mode, cpu.pc); 
}
uint16_t cpu_get_address(enum addressmode mode, uint16_t addr) {
    bool prev_page_crossed = cpu.page_crossed; 
    uint16_t target = get_address_vaddr(mode, addr); 
    cpu.page_crossed = prev_page_crossed; 
    return target; 
}
INLINE void tick_cycle(unsigned tick) {
    cpu.delayed_cycles += tick; 
}
void handle_delayed_cycles(void) {
    // tick bus by cpu.delayed_cycles 
    bus_catch_up_cpu_cycles(cpu.delayed_cycles); 
    cpu.delayed_cycles = 0; 
}
INLINE void update_flags_zn(uint8_t result) {
    if (result == 0) {
        cpu.p |= (uint8_t)flag_zero; 
    } else {
        cpu.p &= ~((uint8_t)flag_zero);
    }
    if ((result >> 7) != 0) {
        cpu.p |= (uint8_t)flag_negative;
    } else {
        cpu.p &= ~((uint8_t)flag_negative); 
    }
}
static inline void set_A(uint8_t val) {
    cpu.a = val; 
    update_flags_zn(val); 
}
INLINE void ldy(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    cpu.y = data; 
    update_flags_zn(data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void ldx(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    cpu.x = data; 
    update_flags_zn(data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void lda(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    cpu.a = data; 
    update_flags_zn(data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void sta(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    cpu_mem_write(addr, cpu.a); 
}
INLINE void and(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    set_A(cpu.a & data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void eor(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    set_A(cpu.a ^ data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void ora(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    set_A(cpu.a | data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void tax(void) {
    cpu.x = cpu.a; 
    update_flags_zn(cpu.x); 
}
INLINE void inx(void) {
    cpu.x += UINT8_C(1); 
    update_flags_zn(cpu.x); 
}
INLINE void iny(void) {
    cpu.y += UINT8_C(1); 
    update_flags_zn(cpu.y); 
}
INLINE void set_carry(void) {
    cpu.p |= (uint8_t)flag_carry; 
}
INLINE void clear_carry(void) {
    cpu.p &= ~((uint8_t)flag_carry); 
}
INLINE void add_to_A(uint8_t val) {
    uint16_t sum = (uint16_t)cpu.a + (uint16_t)val; 
    if (cpu.p & (uint8_t)flag_carry) {
        sum += 1; 
    } 
    if (sum > UINT16_C(0xff)) {
        set_carry();
    } else {
        clear_carry(); 
    }
    uint8_t result = (uint8_t)sum; 
    if (((val ^ result) & (result ^ cpu.a) & 0x80) != 0) {
        cpu.p |= (uint8_t)flag_overflow; 
    } else {
        cpu.p &= ~((uint8_t)flag_overflow); 
    }
    set_A(result); 
}
INLINE void sbc(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    int8_t value = cpu_mem_read(addr); 
    add_to_A((uint8_t)((-value) - 1)); // 6502 uses two's complement 
    // also notice that the borrow bit is the negation of the carry flag, so we need to pre-subtract 1 from the two's complement 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void adc(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t value = cpu_mem_read(addr); 
    add_to_A(value); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE uint8_t pop_stack(void) {
    cpu.sp += 1; 
    return cpu_mem_read(STACK_OFFSET + (uint16_t)cpu.sp); 
}
INLINE void push_stack(uint8_t val) {
    cpu_mem_write(STACK_OFFSET + (uint16_t)cpu.sp, val); 
    cpu.sp -= 1; 
}
INLINE void push_stack_uint16(uint16_t val) {
    push_stack((uint8_t)(val >> 8));
    push_stack((uint8_t)(val & 0xff)); 
}
INLINE uint16_t pop_stack_uint16(void) {
    uint16_t lo = (uint16_t)pop_stack(); 
    uint16_t hi = (uint16_t)pop_stack(); 
    return ((hi << 8) | lo);
}
INLINE void asl_A(void) {
    uint8_t value = cpu.a; 
    if (value >> 7) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    value <<= 1; 
    set_A(value); 
}
INLINE uint8_t asl(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    if (data >> 7) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data <<= 1; 
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
}
INLINE void lsr_A(void) {
    uint8_t data = cpu.a; 
    if (data & 1) {
        set_carry();
    } else {
        clear_carry(); 
    }
    data >>= 1; 
    set_A(data); 
}
INLINE uint8_t lsr(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    if (data & 1) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data >>= 1; 
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
}
INLINE void rol_A(void) {
    uint8_t data = cpu.a; 
    uint8_t carry_bit = (cpu.p & (uint8_t)flag_carry);
    if (data >> 7) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data <<= 1; 
    if (carry_bit) {
        data |= 1; 
    }
    set_A(data); 
}
INLINE uint8_t rol(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    uint8_t carry_bit = (cpu.p & (uint8_t)flag_carry);
    if (data >> 7) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data <<= 1; 
    if (carry_bit) {
        data |= 1; 
    }
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
}
INLINE void ror_A(void) {
    uint8_t data = cpu.a; 
    uint8_t carry_bit = (cpu.p & (uint8_t)flag_carry);
    if (data & 1) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data >>= 1; 
    if (carry_bit) {
        data |= UINT8_C(0b10000000); 
    }
    set_A(data); 
}
INLINE uint8_t ror(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    uint8_t carry_bit = (cpu.p & (uint8_t)flag_carry);
    if (data & 1) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    data >>= 1; 
    if (carry_bit) {
        data |= UINT8_C(0b10000000); 
    }
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
}
INLINE uint8_t inc(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    data += 1; 
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
} 
INLINE void dey(void) {
    cpu.y -= 1; 
    update_flags_zn(cpu.y); 
}
INLINE void dex(void) {
    cpu.x -= 1; 
    update_flags_zn(cpu.x); 
}
INLINE uint8_t dec(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    data -= 1; 
    cpu_mem_write(addr, data); 
    update_flags_zn(data); 
    return data; 
}
INLINE void pla(void) {
    uint8_t data = pop_stack(); 
    set_A(data); 
}
INLINE void plp(void) {
    cpu.p = pop_stack(); 
    cpu.p &= ~((uint8_t)flag_break); 
    cpu.p |= (uint8_t)flag_break2; 
}
INLINE void php(void) {
    uint8_t flags = cpu.p; 
    flags |= (uint8_t)flag_break; 
    flags |= (uint8_t)flag_break2; 
    push_stack(flags); 
}
INLINE void bit(enum addressmode mode) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    uint8_t and = cpu.a & data; 
    if (!and) {
        cpu.p |= (uint8_t)flag_zero; 
    } else {
        cpu.p &= ~((uint8_t)flag_zero); 
    }
    if (data & UINT8_C(0b10000000)) {
        cpu.p |= (uint8_t)flag_negative; 
    } else {
        cpu.p &= ~((uint8_t)flag_negative); 
    }
    if (data & UINT8_C(0b01000000)) {
        cpu.p |= (uint8_t)flag_overflow; 
    } else {
        cpu.p &= ~((uint8_t)flag_overflow); 
    }
}
INLINE void compare(enum addressmode mode, uint8_t compare_with) {
    uint16_t addr = get_address(mode); 
    uint8_t data = cpu_mem_read(addr); 
    if (data <= compare_with) {
        set_carry(); 
    } else {
        clear_carry(); 
    }
    update_flags_zn(compare_with - data); 
    if (cpu.page_crossed) {
        tick_cycle(1); 
    }
}
INLINE void branch(uint8_t condition) {
    if (condition) {
        tick_cycle(1); 
        int8_t offset = cpu_mem_read(cpu.pc); 
        uint16_t target = cpu.pc + 1 + (uint16_t)((int16_t)offset); 
        if (page_crossed(cpu.pc + 1, target)) {
            tick_cycle(1); 
        }
        cpu.pc = target; 
        // if(offset > 0)
        //     printf("pc=%04x taken target=0x%04x offset=0x%02x\n", cpu.pc, target, offset);
    }
}
INLINE void raise_interrupt(enum cpu_interrupt irq) {
    push_stack_uint16(cpu.pc); 
    uint8_t flags = cpu.p; 
    struct cpu_interrupt_info info = irq_info[irq]; 
    if (info.flag_mask & flag_break) {
        set_mask_inplace(flags, flag_break); 
    } else {
        clear_mask_inplace(flags, flag_break); 
    }
    if (info.flag_mask & flag_break2) {
        set_mask_inplace(flags, flag_break2); 
    } else {
        clear_mask_inplace(flags, flag_break2); 
    }
    push_stack(flags); 
    set_mask_inplace(cpu.p, flag_interrupt_disable); 
    tick_cycle(info.cycles); 
    cpu.pc = cpu_mem_read16(INTERRUPT_VECTOR_BASE+irq*UINT16_C(2)); 
}
void cpu_run_with_callback(void(*func)(void)) {
    // int cnt = 0; 
    while (1) {
        // if (cnt++ > 100) {
        //     return; 
        // }
        if (bus_poll_nmi()) {
            raise_interrupt(cpu_interrupt_nmi); 
        }
        func(); 
        uint8_t opcode = cpu_mem_read(cpu.pc); 
        // printf("step pc = %04x opcode=0x%02x\n", cpu.pc, opcode); 
        cpu.pc += 1; 
        uint16_t pc_backup = cpu.pc; 
        struct cpu_opcode op = opcodes[opcode]; 
        if (!op.length) {
            printf("Error: cpu_run unknown opcode %x\n", opcode); 
            return; 
        }
        switch (op.instruction) {
        /* LDA */
        // case 0xa9: 
        // case 0xa5: 
        // case 0xb5: 
        // case 0xad: 
        // case 0xbd: 
        // case 0xb9: 
        // case 0xa1: 
        // case 0xb1: 
        case LDA:
            lda(op.addrmode); 
            break; 
        // case 0xAA: // TAX 
        case TAX:
            tax();
            break;
        // case 0xe8: // INX 
        case INX: 
            inx(); 
            break; 
        // case 0x00: // BRK 
        case BRK: 
            // printf("BRK occurred at pc=0x%04x\n", cpu.pc); 
            return; 
        
        // case 0xd8: // CLD 
        case CLD: 
            cpu.p &= ~((uint8_t)flag_decimal_mode); 
            break; 
        // case 0x58: // CLI 
        case CLI: 
            cpu.p &= ~((uint8_t)flag_interrupt_disable); 
            break; 
        // case 0xb8: // CLV 
        case CLV: 
            cpu.p &= ~((uint8_t)flag_overflow); 
            break; 
        // case 0x18: // CLC 
        case CLC: 
            clear_carry(); 
            break; 
        // case 0x38: // SEC 
        case SEC: 
            set_carry(); 
            break; 
        // case 0x78: // SEI 
        case SEI: 
            cpu.p |= (uint8_t)flag_interrupt_disable; 
            break; 
        // case 0xf8: // SED 
        case SED: 
            cpu.p |= (uint8_t)flag_decimal_mode; 
            break; 
        // case 0x48: // PHA 
        case PHA: 
            push_stack(cpu.a); 
            break; 
        // case 0x68: // PLA 
        case PLA: 
            pla();
            break; 
        // case 0x08: // PHP 
        case PHP: 
            php();
            break; 
        // case 0x28: // PLP 
        case PLP: 
            plp();
            break; 
        /* ADC */
        // case 0x69:
        // case 0x65: 
        // case 0x75: 
        // case 0x6d: 
        // case 0x7d: 
        // case 0x79: 
        // case 0x61: 
        // case 0x71: 
        case ADC: 
            adc(op.addrmode);
            break; 
        

        /* SBC */
        // case 0xe9:
        // case 0xe5: 
        // case 0xf5: 
        // case 0xed: 
        // case 0xfd: 
        // case 0xf9: 
        // case 0xe1: 
        // case 0xf1: 
        case SBC: 
            sbc(op.addrmode); 
            break; 
        
        /* AND */
        // case 0x29:
        // case 0x25: 
        // case 0x35: 
        // case 0x2d: 
        // case 0x3d: 
        // case 0x39: 
        // case 0x21: 
        // case 0x31: 
        case AND: 
            and(op.addrmode);
            break; 


        /* EOR */
        // case 0x49:
        // case 0x45: 
        // case 0x55: 
        // case 0x4d: 
        // case 0x5d: 
        // case 0x59: 
        // case 0x41: 
        // case 0x51: 
        case EOR: 
            eor(op.addrmode);
            break; 

        /* ORA */
        // case 0x09: 
        // case 0x05: 
        // case 0x15: 
        // case 0x0d: 
        // case 0x1d: 
        // case 0x19: 
        // case 0x01: 
        // case 0x11: 
        case ORA: 
            ora(op.addrmode);
            break; 

        // case 0x4a: // LSR 
        case LSR_A: 
            lsr_A(); 
            break; 
        /* LSR */
        // case 0x46: 
        // case 0x56: 
        // case 0x4e: 
        // case 0x5e: 
        case LSR: 
            lsr(op.addrmode);
            break; 

        // case 0x0a: // ASL 
        case ASL_A: 
            asl_A(); 
            break; 

        /* ASL */
        // case 0x06:
        // case 0x16: 
        // case 0x0e: 
        // case 0x1e: 
        case ASL: 
            asl(op.addrmode);
            break; 

        // case 0x2a: // ROL accumulator 
        case ROL_A: 
            rol_A(); 
            break; 

        /* ROL */
        // case 0x26: 
        // case 0x36: 
        // case 0x2e: 
        // case 0x3e: 
        case ROL: 
            rol(op.addrmode);
            break; 


        // case 0x6a: // ROR accumulator 
        case ROR_A: 
            ror_A(); 
            break; 

        /* ROR */
        // case 0x66: 
        // case 0x76: 
        // case 0x6e: 
        // case 0x7e: 
        case ROR: 
            ror(op.addrmode);
            break; 

        /* INC */
        // case 0xe6: 
        // case 0xf6: 
        // case 0xee: 
        // case 0xfe: 
        case INC: 
            inc(op.addrmode);
            break; 
        

        /* INY */
        // case 0xc8: 
        case INY: 
            iny(); 
            break; 

        /* DEC */
        // case 0xc6: 
        // case 0xd6: 
        // case 0xce: 
        // case 0xde: 
        case DEC: 
            dec(op.addrmode);
            break; 

        /* DEX */
        // case 0xca: 
        case DEX: 
            dex();
            break; 

        /* DEY */
        // case 0x88:
        case DEY:  
            dey();
            break; 
        
        /* CMP */
        // case 0xc9: 
        // case 0xc5: 
        // case 0xd5: 
        // case 0xcd: 
        // case 0xdd: 
        // case 0xd9: 
        // case 0xc1: 
        // case 0xd1: 
        case CMP: 
            compare(op.addrmode, cpu.a);
            break; 

        /* CPY */
        // case 0xc0: 
        // case 0xc4: 
        // case 0xcc: 
        case CPY: 
            compare(op.addrmode, cpu.y);
            break; 

        /* CPX */
        // case 0xe0: 
        // case 0xe4: 
        // case 0xec: 
        case CPX: 
            compare(op.addrmode, cpu.x); 
            break; 

        /* JMP Absolute */
        // case 0x4c: 
        case JMP_ABS: 
        {
            uint16_t mem_address = cpu_mem_read16(cpu.pc);
            cpu.pc = mem_address;
            break; 
        }

        /* JMP Indirect */
        // case 0x6c: 
        case JMP: 
        {
            uint16_t mem_address = cpu_mem_read16(cpu.pc);
            // 6502 bug mode with with page boundary:
            // if address $3000 contains $40, $30FF contains $80, and $3100 contains $50,
            // the result of JMP ($30FF) will be a transfer of control to $4080 rather than $5080 as you intended
            // i.e. the 6502 took the low byte of the address from $30FF and the high byte from $3000

            uint16_t indirect_ref = 0; 
            if ((mem_address & UINT16_C(0x00FF)) == UINT16_C(0x00FF)) {
                uint16_t lo = (uint16_t)cpu_mem_read(mem_address); 
                uint16_t hi = (uint16_t)cpu_mem_read(mem_address & UINT16_C(0xFF00)); 
                indirect_ref = ((hi << 8) | lo); 
            } else {
                indirect_ref = cpu_mem_read16(mem_address); 
            }
            cpu.pc = indirect_ref; 
            break; 
        }

        /* JSR */
        // case 0x20: 
        case JSR: 
        {
            push_stack_uint16(cpu.pc + 2 - 1);
            uint16_t target_address = cpu_mem_read16(cpu.pc);
            cpu.pc = target_address; 
            // printf("jsr target = %04x\n", target_address);
            break; 
        }

        /* RTS */
        // case 0x60: 
        case RTS: 
            cpu.pc = pop_stack_uint16() + 1;
            break;  

        /* RTI */
        // case 0x40: 
        case RTI: 
        {
            cpu.p = pop_stack(); 
            cpu.p &= ~((uint8_t)flag_break); 
            cpu.p |= (uint8_t)flag_break2; 
            cpu.pc = pop_stack_uint16(); 
            break; 
        }

        /* BNE */
        // case 0xd0: 
        case BNE: 
        {
            branch(
                !(cpu.p & (uint8_t)flag_zero)
            );
            break; 
        }

        /* BVS */
        // case 0x70: 
        case BVS: 
        {
            branch((cpu.p & (uint8_t)flag_overflow) != 0);
            break; 
        }

        /* BVC */
        // case 0x50: 
        case BVC: 
        {
            branch(!(cpu.p & (uint8_t)flag_overflow));
            break; 
        }

        /* BPL */
        // case 0x10: 
        case BPL: 
        {
            branch(!(cpu.p & (uint8_t)flag_negative));
            break; 
        }

        /* BMI */
        // case 0x30: 
        case BMI: 
        {
            branch((cpu.p & (uint8_t)flag_negative) != 0);
            break; 
        }

        /* BEQ */
        // case 0xf0: 
        case BEQ: 
        {
            branch((cpu.p & (uint8_t)flag_zero) != 0);
            break; 
        }

        /* BCS */
        // case 0xb0: 
        case BCS: 
        {
            branch((cpu.p & (uint8_t)flag_carry) != 0);
            break; 
        }

        /* BCC */
        // case 0x90: 
        case BCC: 
        {
            branch(!(cpu.p & (uint8_t)flag_carry));
            break; 
        }

        /* BIT */
        // case 0x24: 
        // case 0x2c: 
        case BIT: 
            bit(op.addrmode);
            break; 
        

        /* STA */
        // case 0x85: 
        // case 0x95: 
        // case 0x8d: 
        // case 0x9d: 
        // case 0x99: 
        // case 0x81: 
        // case 0x91: 
        case STA: 
            sta(op.addrmode);
            break; 

        /* STX */
        // case 0x86: 
        // case 0x96: 
        // case 0x8e: 
        case STX: 
            cpu_mem_write(get_address(op.addrmode), cpu.x);
            break; 

        /* STY */
        // case 0x84: 
        // case 0x94: 
        // case 0x8c: 
        case STY: 
            cpu_mem_write(get_address(op.addrmode), cpu.y);
            break; 

        /* LDX */
        // case 0xa2: 
        // case 0xa6: 
        // case 0xb6: 
        // case 0xae: 
        // case 0xbe: 
        case LDX: 
            ldx(op.addrmode);
            break; 
        

        /* LDY */
        // case 0xa0: 
        // case 0xa4: 
        // case 0xb4: 
        // case 0xac: 
        // case 0xbc: 
        case LDY: 
            ldy(op.addrmode);
            break;

        /* NOP */
        // case 0xea: 
        case NOP: 
            break; 

        /* TAY */
        // case 0xa8: 
        case TAY: 
            cpu.y = cpu.a; 
            update_flags_zn(cpu.y);
            break; 

        /* TSX */
        // case 0xba: 
        case TSX: 
            cpu.x = cpu.sp;
            update_flags_zn(cpu.x);
            break; 

        /* TXA */
        // case 0x8a: 
        case TXA: 
            cpu.a = cpu.x; 
            update_flags_zn(cpu.a);
            break; 

        /* TXS */
        // case 0x9a: 
        case TXS: 
            cpu.sp = cpu.x;
            break; 

        /* TYA */
        // case 0x98: 
        case TYA: 
            cpu.a = cpu.y;
            update_flags_zn(cpu.a);
            break; 


        /* unofficial */

        /// Source: https://github.com/bugzmanov/nes_ebook/blob/master/code/ch5.1/src/cpu.rs

        /* DCP */
        // case 0xc7: 
        // case 0xd7: 
        // case 0xcf: 
        // case 0xdf: 
        // case 0xdb: 
        // case 0xd3: 
        // case 0xc3: 
        case _DCP: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            data -= 1; 
            cpu_mem_write(addr, data); 
            if (data <= cpu.a) {
                set_carry(); 
            }
            update_flags_zn(cpu.a - data); 
            break; 
        }

        /* RLA */
        // case 0x27: 
        // case 0x37: 
        // case 0x2F: 
        // case 0x3F: 
        // case 0x3b: 
        // case 0x33: 
        // case 0x23: 
        case _RLA: 
        {
            uint8_t data = rol(op.addrmode); 
            set_A(data & cpu.a); 
            break; 
        }

        /* SLO */ // tests? 
        // case 0x07: 
        // case 0x17: 
        // case 0x0F: 
        // case 0x1f: 
        // case 0x1b: 
        // case 0x03: 
        // case 0x13: 
        case _SLO: 
        {
            uint8_t data = asl(op.addrmode); 
            set_A(data | cpu.a); 
            break; 
        }

        /* SRE */ // tests? 
        // case 0x47: 
        // case 0x57: 
        // case 0x4F: 
        // case 0x5f: 
        // case 0x5b: 
        // case 0x43: 
        // case 0x53: 
        case _SRE: 
        {
            uint8_t data = lsr(op.addrmode); 
            set_A(data ^ cpu.a); 
            break; 
        }

        /* SKB */
        // case 0x80: 
        // case 0x82: 
        // case 0x89: 
        // case 0xc2: 
        // case 0xe2:  
        case _SKB: 
        {
            /* 2 byte NOP */
            // ? 
            break; 
        }

        /* AXS */
        // case 0xCB:  
        case _AXS: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            uint8_t xa = cpu.x & cpu.a; 
            uint8_t result = xa - data; 
            if (data <= xa) {
                set_carry(); 
            }
            update_flags_zn(result); 
            cpu.x = result; 
            break; 
        }

        /* ARR */
        // case 0x6B:  
        case _ARR: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            set_A(data & cpu.a); 
            ror_A(); 
            uint8_t result = cpu.a; 
            uint8_t b5 = (result >> 5) & 1; 
            uint8_t b6 = (result >> 6) & 1; 
            if (b6) {
                set_carry(); 
            } else {
                clear_carry(); 
            }
            if (b5 != b6) {
                cpu.p |= (uint8_t)flag_overflow; 
            } else {
                cpu.p &= ~((uint8_t)flag_overflow); 
            }
            // registers? 
            update_flags_zn(result); 
            break; 
        }

        /* unofficial SBC */
        // case 0xeb: 
        case _SBC: 
        {
            uint16_t addr = get_address(op.addrmode); 
            int8_t data = cpu_mem_read(addr); 
            add_to_A((uint8_t)(-data - INT8_C(1))); 
            break; 
        }

        /* ANC */
        // case 0x0b: 
        // case 0x2b: 
        case _ANC: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            set_A(data & cpu.a); 
            if (cpu.p & (uint8_t)flag_negative) {
                set_carry(); 
            } else {
                clear_carry(); 
            }
            break; 
        }

        /* ALR */
        // case 0x4b: 
        case _ALR: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            set_A(data & cpu.a); 
            lsr_A(); 
            break; 
        }

        // test for everything below

        /* NOP read */
        // case 0x04: 
        // case 0x44: 
        // case 0x64: 
        // case 0x14: 
        // case 0x34: 
        // case 0x54: 
        // case 0x74: 
        // case 0xd4: 
        // case 0xf4: 
        // case 0x0c: 
        // case 0x1c: 
        // case 0x3c: 
        // case 0x5c: 
        // case 0x7c: 
        // case 0xdc: 
        // case 0xfc: 
        case _NOP_R: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            if (cpu.page_crossed) {
                tick_cycle(1); 
            }
            break; 
        }

        /* RRA */
        // case 0x67: 
        // case 0x77: 
        // case 0x6f: 
        // case 0x7f: 
        // case 0x7b: 
        // case 0x63: 
        // case 0x73: 
        case _RRA: 
        {
            uint8_t data = ror(op.addrmode); 
            add_to_A(data); 
            break; 
        }

        /* ISB */
        // case 0xe7: 
        // case 0xf7: 
        // case 0xef: 
        // case 0xff: 
        // case 0xfb: 
        // case 0xe3: 
        // case 0xf3: 
        case _ISB: 
        {
            int8_t data = (int8_t)inc(op.addrmode); 
            add_to_A((uint8_t)(-data - INT8_C(1))); 
            break; 
        }

        /* NOPs */
        // case 0x02: 
        // case 0x12: 
        // case 0x22: 
        // case 0x32: 
        // case 0x42: 
        // case 0x52: 
        // case 0x62: 
        // case 0x72: 
        // case 0x92: 
        // case 0xb2: 
        // case 0xd2: 
        // case 0xf2: 
            // break; 

        // case 0x1a: 
        // case 0x3a: 
        // case 0x5a: 
        // case 0x7a: 
        // case 0xda: 
        // case 0xfa: 
        case _NOP: 
            break; 

        /* LAX */
        // case 0xa7: 
        // case 0xb7: 
        // case 0xaf: 
        // case 0xbf: 
        // case 0xa3: 
        // case 0xb3: 
        case _LAX: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            set_A(data); 
            cpu.x = cpu.a; 
            break; 
        }

        /* SAX */
        // case 0x87: 
        // case 0x97: 
        // case 0x8f: 
        // case 0x83: 
        case _SAX: 
        {
            uint8_t data = cpu.a & cpu.x; 
            uint16_t addr = get_address(op.addrmode); 
            cpu_mem_write(addr, data); 
            break; 
        }

        /* LXA */
        // case 0xab: 
        case _LXA: 
        {
            lda(op.addrmode); 
            tax(); 
            break; 
        }

        /* XAA */
        // case 0x8b: 
        case _XAA: 
        {
            cpu.a = cpu.x; 
            update_flags_zn(cpu.a); 
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            set_A(data & cpu.a); 
            break; 
        }

        /* LAS */
        // case 0xbb: 
        case _LAS: 
        {
            uint16_t addr = get_address(op.addrmode); 
            uint8_t data = cpu_mem_read(addr); 
            data &= cpu.sp;
            cpu.a = data; 
            cpu.x = data; 
            cpu.sp = data; 
            update_flags_zn(data); 
            break; 
        }

        /* TAS */
        // case 0x9b: 
        case _TAS: 
        {
            uint8_t data = cpu.a & cpu.x; 
            cpu.sp = data; 
            uint16_t mem_address = cpu_mem_read16(cpu.pc) + (uint16_t)cpu.y; 

            data = ((uint8_t)(mem_address >> 8) + 1) & cpu.sp;
            cpu_mem_write(mem_address, data);
            break; 
        }

        /* AHX  Indirect Y */
        // case 0x93: 
        case _AHX_IND_Y: 
        {
            uint8_t pos = cpu_mem_read(cpu.pc); 
            uint16_t mem_address = cpu_mem_read16(pos) + (uint16_t)cpu.y; 
            uint8_t data = cpu.a & cpu.x & (uint8_t)(mem_address >> 8); 
            cpu_mem_write(mem_address, data); 
            break; 
        }

        /* AHX Absolute Y*/
        // case 0x9f: 
        case _AHX_ABS_Y: 
        {
            uint16_t mem_address = cpu_mem_read16(cpu.pc) + (uint16_t)cpu.y; 
            uint8_t data = cpu.a & cpu.x & (uint8_t)(mem_address >> 8); 
            cpu_mem_write(mem_address, data); 
            break; 
        }

        /* SHX */
        // case 0x9e: 
        case _SHX: 
        {
            uint16_t mem_address = cpu_mem_read16(cpu.pc) + (uint16_t)cpu.y; 
            // if cross page boundry {
            //     mem_address &= ((uint16_t)cpu.x) << 8;
            // }
            uint8_t data = cpu.x & ((uint8_t)(mem_address >> 8) + 1); 
            cpu_mem_write(mem_address, data); 
            break; 
        }

        /* SHY */
        // case 0x9c: 
        case _SHY: 
        {
            uint16_t mem_address = cpu_mem_read16(cpu.pc) + (uint16_t)cpu.x; 
            uint8_t data = cpu.y & ((uint8_t)(mem_address >> 8) + 1); 
            cpu_mem_write(mem_address, data); 
            break; 
        }
        default: 
            printf("Error: cpu_run unhandled opcode %x\n", opcode); 
            break; 
        }
        tick_cycle(op.cycles); 
        // handle cycle ticks? 
        handle_delayed_cycles(); 
        if (pc_backup == cpu.pc) {
            cpu.pc += (uint16_t)(op.length - 1); 
        }
    }
}