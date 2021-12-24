# i686 specific startup kernel code

# constants for multiboot header
.set MB_MAGIC,              0x1BADB002
.set MB_FLAG_PAGE_ALIGN,    1 << 0
.set MB_FLAG_MEMORY_INFO,   1 << 1
.set MB_FLAG_GRAPHICS,      1 << 2
.set MB_FLAGS,              MB_FLAG_PAGE_ALIGN | MB_FLAG_MEMORY_INFO
.set MB_CHECKSUM,           -(MB_MAGIC + MB_FLAGS)

.section .text
    # multiboot header
    .align 4
    .long MB_MAGIC
    .long MB_FLAGS
    .long MB_CHECKSUM
    # unused header fileds filled with zeros:
    .long 0x00000000 # header_addr
    .long 0x00000000 # load_addr
    .long 0x00000000 # load_end_addr
    .long 0x00000000 # bss_end_addr
    .long 0x00000000 # entry_addr

.global start
.type start, @function

.extern arch_kmain
.type arch_kmain, @function

start:
    cli 				     # Block interrupts
    mov     $stack_top, %esp # Set up the stack
    and     $-16, %esp       # Make sure our stack is 16-byte aligned
    pushl   %eax             # Multiboot magic number
    pushl   %ebx             # Multiboot structure
    call    arch_kmain
    hlt 				     # Halt the CPU

.section .bss
.skip 65535   # reserve 64 kib for stack
stack_top:    # this is stack top, stack grows downwards

