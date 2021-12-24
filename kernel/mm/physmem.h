#pragma once

#include <kernel/types.h>
#include "memlayout.h"
#include <kernel/io/fdo.h>

namespace physmem {

// extern uintptr_t phys_bitmap_start;
// extern uintptr_t phys_bitmap_end;

struct free_map_entry_t {
    uintptr_t addr;
    size_t length;
};

void free_pages(uintptr_t addr, size_t n);
void mark_pages_as_used(uintptr_t addr, size_t n);
uintptr_t alloc_pages(size_t n);

void test();

void init(uintptr_t bitmap_addr, free_map_entry_t free_map[],
    size_t free_map_length, size_t mem_total);

};