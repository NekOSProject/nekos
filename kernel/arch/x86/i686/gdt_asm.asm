bits 32
section .text
align 4

global gdt_flush

gdt_flush: ; Load GDT
    mov     eax, [esp + 4]
    lgdt    [eax]
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     ss, ax
    mov     gs, ax
    jmp 0x08:.flush
.flush:
	ret	