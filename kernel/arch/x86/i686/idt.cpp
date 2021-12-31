#include "idt.h"

static idt_t idt;

void idt_set_gate(u8 num, idt_gate_t base, u16 sel, u8 flags) {
    idt.entries[num].base_low = ((uintptr_t)base & 0xFFFF);
    idt.entries[num].base_high = ((uintptr_t)base >> 16) & 0xFFFF;
    idt.entries[num].sel = sel;
    idt.entries[num].zero = 0;
    idt.entries[num].flags = flags | 0x60;
}

void idt_install() {
    idt_pointer_t *idtp = &idt.pointer;
    idtp->limit = sizeof idt.entries - 1;
    idtp->base = (u32)&idt.entries[0];
    memset(&idt.entries[0], 0, sizeof idt.entries);

    idt_load(idtp);
}