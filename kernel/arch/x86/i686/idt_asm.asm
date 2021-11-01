bits 32
section .text
align 4

global idt_load

idt_load:
	mov eax, [esp + 4]
	lidt [eax]
	ret