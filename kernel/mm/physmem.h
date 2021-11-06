#pragma once

#include <kernel/types.h>
#include "memlayout.h"
#include <kernel/io/fdo.h>

extern uintptr_t phys_bitmap_start;
extern uintptr_t phys_bitmap_end;

typedef struct {
    uintptr_t addr;
    size_t length;
} physmem_free_map_entry_t;

void physmem_init(uintptr_t bitmap_addr, physmem_free_map_entry_t free_map[], size_t free_map_length, size_t mem_total);
