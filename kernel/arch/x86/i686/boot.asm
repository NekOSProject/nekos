; i686 specific startup kernel code

bits 32
section .text
        ; multiboot header
        align 4
        dd 0x1BADB002              ; magic
        dd 0x00                    ; flags
        dd - (0x1BADB002 + 0x00)   ; checksum. m+f+c should be zero

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

