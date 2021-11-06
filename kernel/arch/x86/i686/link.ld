OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    . = 0x100000;
    kernel_phys_start = .;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
    kernel_phys_end = .;
}