#pragma once
#include <kernel/libk/string.h>
#include <kernel/types.h>
#include "tss.h"

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

extern "C" void gdt_flush(uintptr_t);

void gdt_set_gate(u8 num, u64 base, u64 limit, u8 access, u8 gran);
void gdt_set_gsbase(uintptr_t base);
uintptr_t gdt_get_gsbase(void);
void write_tss(int32_t num, u16 ss0, uint32_t esp0);
void set_kernel_stack(uintptr_t stack);
void gdt_install();
