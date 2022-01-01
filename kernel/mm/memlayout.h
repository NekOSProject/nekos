#pragma once

#include <stdint.h>

static constexpr size_t PAGE_SIZE = 4096;

#define KERNEL_START_PHYS ((uintptr_t)&kernel_phys_start)
#define KERNEL_END_PHYS ((uintptr_t)&kernel_phys_end)
#define KERNEL_SIZE (KERNEL_END_PHYS - KERNEL_START_PHYS)

#define PAGE_ALIGN_DOWN(addr) ((addr) & -PAGE_SIZE)
#define PAGE_ALIGN_UP(addr)   (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1) )

extern uintptr_t kernel_phys_start;
extern uintptr_t kernel_phys_end;
