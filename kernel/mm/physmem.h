#pragma once

#include <kernel/types.h>
#include "memlayout.h"
#include <kernel/io/fdo.h>

extern uintptr_t phys_bitmap_start;
extern uintptr_t phys_bitmap_end;

typedef struct {
    u64 addr;
    u64 length;
} physmem_free_map_entry_t;
