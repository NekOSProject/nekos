#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

#define KERNEL_START_PHYS ((uintptr_t)&kernel_phys_start)
#define KERNEL_END_PHYS ((uintptr_t)&kernel_phys_end)
#define KERNEL_SIZE (KERNEL_END_PHYS - KERNEL_START_PHYS)

#define ALIGN_BLOCK(addr) ((addr) - ((addr) % PHYS_BLOCK_SIZE))

extern uintptr_t kernel_phys_start;
extern uintptr_t kernel_phys_end;
