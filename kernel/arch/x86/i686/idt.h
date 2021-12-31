#pragma once
#include <kernel/libk/string.h>
#include <kernel/types.h>

typedef struct {
    u16 base_low;
    u16 sel;
    u8 zero;
    u8 flags;
    u16 base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_pointer_t;

typedef struct {
    idt_entry_t entries[256];
    idt_pointer_t pointer;
} __attribute__((packed)) idt_t;

using idt_gate_t = void (*)();

extern "C" void idt_load(idt_pointer_t *);

void idt_set_gate(u8 num, idt_gate_t base, u16 sel, u8 flags);
void idt_install();
