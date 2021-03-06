#include "uart.h"
#include "delay.h"
#include "printf.h"
#include "crc.h"

enum uart_boot_msgs {
    RISCV64BASE     = 0x40000000,       // for Allwinner D1 where program gets linked.  we could send this.
    ARM64BASE       = 0x180000,          // where program gets linked.  we could send this.
    BOOT_START      = 0xFFFF0000,
    GET_PROG_INFO   = 0x11112222,       // board sends
    PUT_PROG_INFO   = 0x33334444,       // unix sends
    GET_CODE        = 0x55556666,       // board sends
    PUT_CODE        = 0x77778888,       // unix sends
    BOOT_SUCCESS    = 0x9999AAAA,       // board sends on success
    BOOT_ERROR      = 0xBBBBCCCC,       // board sends on failure.
    PRINT_STRING    = 0xDDDDEEEE,       // board sends to print a string.
    // error codes from the board to unix
    BAD_CODE_ADDR   = 0xdeadbeef,
    BAD_CODE_CKSUM  = 0xfeedface,
};
static uint32_t boot_get32(void) {
    uint32_t u = (uint32_t)uart_getc(UART0_BASE); 
    u |= uart_getc(UART0_BASE) << 8;
    u |= uart_getc(UART0_BASE) << 16;
    u |= uart_getc(UART0_BASE) << 24;
    return u; 
}

static void boot_put32(uint32_t u) {
    uart_putc(UART0_BASE, (u >> 0)  & 0xff);
    uart_putc(UART0_BASE, (u >> 8)  & 0xff);
    uart_putc(UART0_BASE, (u >> 16) & 0xff);
    uart_putc(UART0_BASE, (u >> 24) & 0xff);
}

static unsigned has_data_timeout(unsigned timeout) {
    unsigned starting_usec = get_current_time_us();
    unsigned now_usec = 0;
    while (1) {
        now_usec = get_current_time_us(); 
        // printf("now_usec = %llu\n", now_usec); 
        if (now_usec - starting_usec > timeout) {
            return 1;
        }
        if (uart_has_data(UART0_BASE)) {
            return 0;
        }
    }
    return 1;
}


static void wait_for_data(unsigned usec_timeout) {
    // printf("waiting for data...\n"); 
    while (has_data_timeout(usec_timeout)) {
        // printf("timeout!\n"); 
        boot_put32(GET_PROG_INFO);
    }
}

uintptr_t uart_get_code(uintptr_t target_address) {
    while (1) {
        // 0. keep sending GET_PROG_INFO every 300ms until there's data
        wait_for_data(300 * 1000);
        // 2. expect: [PUT_PROG_INFO, addr, nbytes, cksum] 
        // we echo cksum back in step 4 to help debugging.
        uint32_t put_prog_info = boot_get32();  // tag 
        uint32_t addr          = boot_get32();  // unused here 
        uint32_t nbytes        = boot_get32();  // number of bytes in the binary 
        uint32_t cksum         = boot_get32();  // crc checksum 
        (void)addr;
        if (put_prog_info != PUT_PROG_INFO) {
            boot_put32(BOOT_ERROR);
            continue; 
        }
        uintptr_t code_address = target_address; 
        // 3. If the binary will collide with us, abort. 
        //    you can assume that code must be below where the booloader code
        //    gap starts.
        // code too big that it wraps around, this should not happen anyways 
        if (code_address + nbytes < code_address) {
            boot_put32(BAD_CODE_ADDR);
            continue; 
        }
        // do we want to check the heap? 
        // let's not care about it, however the program uses the heap depends on itself 
        // 4. send [GET_CODE, cksum] back.
        boot_put32(GET_CODE);
        boot_put32(cksum);
        // 5. expect: [PUT_CODE, <code>]
        //  read each sent byte and write it starting at 
        //  <addr> using put8
        uint32_t put_code = boot_get32();
        if (put_code != PUT_CODE) {
            boot_put32(BAD_CODE_ADDR); 
            continue; 
        }
        volatile char *addr_ptr = (volatile char*)(code_address); 
        for (uint32_t i = 0; i < nbytes; ++i) {
            char b = uart_getc(UART0_BASE);
            (*addr_ptr) = b; 
            addr_ptr += 1;
        }
        // 6. verify the cksum of the copied code.
        uint32_t new_cksum = crc32((const void*)(code_address), nbytes);
        if (new_cksum != cksum) {
            boot_put32(BAD_CODE_CKSUM);
            continue; 
        }
        // 7. send back a BOOT_SUCCESS!
        boot_put32(BOOT_SUCCESS);
        const char msg[] = "<Yancheng Ou>: success: Received the program!\n";
        for (unsigned i = 0; i < sizeof(msg)-1; ++i) {
            uart_putc(UART0_BASE, msg[i]); 
        }
        dev_barrier(); 
        delay_ms(1);
        return code_address;
    }

    return 0x00000000UL;
}

