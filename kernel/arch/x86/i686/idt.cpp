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
    uintptr_t base;
} __attribute__((packed)) idt_pointer_t;

static struct {
    idt_entry_t entries[256];
    idt_pointer_t pointer;
} idt __attribute__((used));

typedef void (*idt_gate_t)(void);

extern "C" void idt_load(uintptr_t);

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
    idtp->base = (uintptr_t)&idt.entries[0];
    memset(&idt.entries[0], 0, sizeof idt.entries);

    idt_load((uintptr_t)idtp);
}