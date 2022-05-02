#include <kernel/types.h>

#include "../vgatext.h"
#include "../debug.h"
#include <kernel/libk/assert.h>
#include <kernel/multiboot.h>
#include <kernel/io/fdo.h>
#include <kernel/mm/memlayout.h>
#include <kernel/mm/physmem.h>
#include <kernel/kernel.h>
#include "gdt.h"
#include "idt.h"

extern "C" void arch_kmain(struct multiboot* mboot, u32 mboot_magic) {

    printk_log("Hello, World!\n");

    vgatext_init((void *)0xB8000);

    assert(mboot_magic == MULTIBOOT_EAX_MAGIC);
    (void)mboot;

    printk_dup(FDO_ARCH_i686 "Hello from arch_kmain\n");

    gdt_install();
    printk_dup(FDO_ARCH_i686 "GDT installed\n");

    idt_install();
    printk_dup(FDO_ARCH_i686 "IDT installed\n");

    printk_dup(FDO_ARCH_i686 "kernel_phys_start = %p\n" FDO_ARCH_i686 "kernel_phys_end = %p\n", KERNEL_START_PHYS, KERNEL_END_PHYS);

    #define DONT_USE_UP_TO 0x100000

    // parse memory map from grub
    mboot_memmap_t *memmap = (mboot_memmap_t *)mboot->mmap_addr;
    int memmap_entries_count = mboot->mmap_length / sizeof(mboot_memmap_t);
    physmem::free_map_entry_t phys_free_map[memmap_entries_count];
    int phys_free_map_len = 0;
    size_t mem_total = 0;
    for (int i = 0; i < memmap_entries_count; i++) {
        if (memmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            if (memmap[i].base_addr + memmap[i].length >= DONT_USE_UP_TO) {
                phys_free_map[phys_free_map_len++] = (physmem::free_map_entry_t){(uintptr_t)memmap[i].base_addr, (size_t)memmap[i].length};
            }
        }
        mem_total += (size_t)memmap[i].length;
    }

    uintptr_t bitmap_addr = 0;
    printk_dup(FDO_ARCH_i686 "phys free map:\n");
    for (int i = 0; i < phys_free_map_len; i++) {
        printk_dup("    base = %p, len = %zu\n", phys_free_map[i].addr, phys_free_map[i].length);
        uintptr_t left_addr = phys_free_map[i].addr;
        uintptr_t right_addr = left_addr + phys_free_map[i].length - 1;
        if (left_addr <= KERNEL_START_PHYS && KERNEL_END_PHYS <= right_addr) {
            if (right_addr - PAGE_ALIGN_UP(KERNEL_END_PHYS) + 1 >= mem_total / 8 ) { // mem_total/8 is phys mm bitmap size
                bitmap_addr = PAGE_ALIGN_UP(KERNEL_END_PHYS);
                break;
            } else {
                printk_dup("Cannot place phys bitmap after kernel. Halting...\n");
                while (1) {}
            }
        }
    }
    printk_dup("\n");

    physmem::init(bitmap_addr, phys_free_map, phys_free_map_len, mem_total);

    kernel::kmain();
}

