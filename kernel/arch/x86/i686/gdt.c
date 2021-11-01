#include "tss.h"
#include <kernel/libk/string.h>
#include <kernel/types.h>

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    u16 limit;
    uintptr_t base;
} __attribute__((packed)) gdt_pointer_t;

static struct {
    gdt_entry_t entries[7];
    gdt_pointer_t pointer;
    tss_entry_t tss;
} gdt __attribute__((used));

extern void gdt_flush(uintptr_t);

void gdt_set_gate(u8 num, u64 base, u64 limit, u8 access, u8 gran) {
    gdt.entries[num].base_low = (base & 0xFFFF);
    gdt.entries[num].base_middle = (base >> 16) & 0xFF;
    gdt.entries[num].base_high = (base >> 24) & 0xFF;

    gdt.entries[num].limit_low = (limit & 0xFFFF);
    gdt.entries[num].granularity = (limit >> 16) & 0X0F;

    gdt.entries[num].granularity |= (gran & 0xF0);

    gdt.entries[num].access = access;
}

void gdt_set_gsbase(uintptr_t base) {
    gdt.entries[6].base_low = (base & 0xFFFF);
    gdt.entries[6].base_middle = (base >> 16) & 0xFF;
    gdt.entries[6].base_high = (base >> 24) & 0xFF;
    asm volatile("mov %0, %%gs" ::"r"((6 << 3) | 0x3));
}

uintptr_t gdt_get_gsbase(void) {
    return (gdt.entries[6].base_low) | (gdt.entries[6].base_middle << 16) |
           (gdt.entries[6].base_high << 24);
}

static void write_tss(int32_t num, u16 ss0, uint32_t esp0);

void gdt_install(void) {
    gdt_pointer_t *gdtp = &gdt.pointer;
    gdtp->limit = sizeof gdt.entries - 1;
    gdtp->base = (uintptr_t)&gdt.entries[0];

    gdt_set_gate(0, 0, 0, 0, 0);                // NULL segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User data
    write_tss(5, 0x10, 0x0);
    gdt_set_gate(6, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uintptr_t)gdtp);
    tss_flush();
}

static void write_tss(int32_t num, u16 ss0, uint32_t esp0) {
    tss_entry_t *tss = &gdt.tss;
    uintptr_t base = (uintptr_t)tss;
    uintptr_t limit = base + sizeof *tss;

    // Add the TSS descriptor to the GDT
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    memset(tss, 0x0, sizeof *tss);

    tss->ss0 = ss0;
    tss->esp0 = esp0;
    tss->cs = 0x0b;
    tss->ss = 0x13;
    tss->ds = 0x13;
    tss->es = 0x13;
    tss->fs = 0x13;
    tss->gs = 0x13;

    tss->iomap_base = sizeof *tss;
}

void set_kernel_stack(uintptr_t stack) { gdt.tss.esp0 = stack; }
