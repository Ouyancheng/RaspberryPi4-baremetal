.section ".text.boot"

.global _start 

_start: 
    // mpidr_el1: Multiprocessor Affinity Register, EL1
    // see ARM A72 MPCore page 4-91 
    mrs x1, mpidr_el1 
    // the last 2 bits are CPU ID 
    and x1, x1, #0b11 
    // make sure we are on core 0 
    cbz x1, incore0 

hang:  
    // power save 
    wfe 
    b hang 

incore0: 
    ldr x1, =_start
    mov     sp, x1
////////// clear BSS 
    ldr     x1, =__bss_start__ 
    /// NOTE: the bss size is the number of int64 
    ldr     w2, =__bss_size__ 
bssclearloop: 
    cbz     w2, bootup 
    /// xzr: the always-zero register 
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, bssclearloop  

bootup: 
    bl main 
    bl rpi_reboot
    b hang 


// .global dev_barrier
// dev_barrier: 
//     dsb sy
//     ret

