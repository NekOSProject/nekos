OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    . = 0x1M;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}