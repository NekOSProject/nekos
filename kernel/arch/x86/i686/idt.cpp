#include "idt.h"

static idt_t idt;

void idt_set_gate(u8 num, void* isr, u16 cs, u8 flags) {
    idt.entries[num].isr_low = ((uintptr_t)isr & 0xFFFF);
    idt.entries[num].isr_high = ((uintptr_t)isr >> 16) & 0xFFFF;
    idt.entries[num].cs = cs;
    idt.entries[num].reserved = 0;
    idt.entries[num].flags = flags;
}

void idt_install() {
    idt.idtr.limit = IDT_MAX_DESCRIPTORS - 1;
    idt.idtr.base = (u32)&idt.entries[0];
    memset(&idt.entries[0], 0, IDT_MAX_DESCRIPTORS);
    
    idt_load(&idt.idtr);
}