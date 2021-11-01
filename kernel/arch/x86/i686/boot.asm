; i686 specific startup kernel code

MB_MAGIC            equ 0x1BADB002
MB_FLAG_PAGE_ALIGN  equ  1 << 0
MB_FLAG_MEMORY_INFO equ  1 << 1
MB_FLAG_GRAPHICS    equ  1 << 2
MB_FLAGS            equ  MB_FLAG_PAGE_ALIGN | MB_FLAG_MEMORY_INFO
MB_CHECKSUM         equ  -(MB_MAGIC + MB_FLAGS)

bits 32
section .text
        ; multiboot header
        align 4
        dd MB_MAGIC
        dd MB_FLAGS
        dd MB_CHECKSUM
        ; unused header fileds filled with zeros:
        dd 0 ; header_addr
        dd 0 ; load_addr
        dd 0 ; load_end_addr
        dd 0 ; bss_end_addr
        dd 0 ; entry_addr

extern kmain

global start

start:
	cli 				; block interrupts
	mov esp, stack_space
    push eax ; Multiboot magic number
	push ebx ; Multiboot structure
	call kmain
	hlt 				; halt the CPU

section .bss
resb 65535   ; 64 kib for stack
stack_space: ; this is stack top, stack grows downwards

