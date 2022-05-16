SECTIONS
{
    . = 0x180000;     /* Kernel load address for AArch64 */
    .text : { 
        PROVIDE(__code_start__ = .); 
        KEEP(*(.text.boot)) 
        *(.text .text.* .gnu.linkonce.t*) 
        PROVIDE(__code_end__ = .); 
        . = ALIGN(8); 
    }
    .rodata : { 
        *(.rodata .rodata.* .gnu.linkonce.r*) 
    }
    PROVIDE(_data = .);
    
    .data : { 
        PROVIDE(__data_start__ = .); 
        *(.data .data.* .gnu.linkonce.d*) 
        . = ALIGN(4);
        PROVIDE(__data_end__ = .);
    }
    
    .bss (NOLOAD) : {
        . = ALIGN(16);
        PROVIDE(__bss_start__ = .);
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        PROVIDE(__bss_end__ = .);
        . = ALIGN(8);
    }
    PROVIDE(__prog_end__ = .);
    PROVIDE(__heap_start__ = .);
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size__ = (__bss_end__ - __bss_start__) >> 3;

