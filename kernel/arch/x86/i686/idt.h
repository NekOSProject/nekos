#pragma once
#include <kernel/libk/string.h>
#include <kernel/types.h>

static constexpr size_t IDT_MAX_DESCRIPTORS = 256;

typedef struct {
    u16 isr_low;   // The lower 16 bits of the ISR's address
    u16 cs;        // The GDT segment selector that the CPU will load into CS before calling the ISR
    u8  reserved;  // Reserved, set to zero
    u8  flags;     // Type and attributes; see the IDT page
    u16 isr_high;  // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idtr_t;

typedef struct {
    idt_entry_t entries[IDT_MAX_DESCRIPTORS];
    idtr_t idtr;
} idt_t;

using idt_gate_t = void (*)();

extern "C" void idt_load(idtr_t *);

void idt_set_gate(u8 num, idt_gate_t base, u16 sel, u8 flags);
void idt_install();
