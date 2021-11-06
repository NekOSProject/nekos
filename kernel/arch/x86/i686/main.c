#include <kernel/types.h>

#include "../vgatext.h"
#include "../debug.h"
#include <kernel/libk/assert.h>
#include <kernel/multiboot.h>
#include <kernel/io/fdo.h>
#include <kernel/mm/memlayout.h>
#include <kernel/mm/physmem.h>
#include <kernel/generic.h>

extern void gdt_install(void);
extern void idt_install(void);

void kmain(struct multiboot* mboot, u32 mboot_magic) {
    #define PREF "[ARCH i686]: "

    printk_log("Hello, world!\n");

    vgatext_init((void *)0xB8000);

    assert(mboot_magic == MULTIBOOT_EAX_MAGIC);
    (void)mboot;

    printk_dup(PREF "Hello from kmain\n");

    gdt_install();
    printk_dup(PREF "GDT installed\n");

    idt_install();
    printk_dup(PREF "IDT installed\n");

    printk_dup(PREF "kernel_phys_start = %x\n" PREF "kernel_phys_end = %x\n", KERNEL_START_PHYS, KERNEL_END_PHYS);

    #define DONT_USE_UP_TO 0x100000

    // parse memory map from grub
    mboot_memmap_t *memmap = (mboot_memmap_t *)mboot->mmap_addr;
    int memmap_entries_count = mboot->mmap_length / sizeof(mboot_memmap_t);
    physmem_free_map_entry_t phys_free_map[memmap_entries_count];
    int phys_free_map_len = 0;
    size_t mem_total = 0;
    for (int i = 0; i < memmap_entries_count; i++) {
        if (memmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            if (memmap[i].base_addr + memmap[i].length >= DONT_USE_UP_TO) {
                phys_free_map[phys_free_map_len++] = (physmem_free_map_entry_t){memmap[i].base_addr, memmap[i].length};
            }
        }
        mem_total += (size_t)memmap[i].length;
    }

    uintptr_t bitmap_addr;
    printk_dup(PREF "phys free map:\n");
    for (int i = 0; i < phys_free_map_len; i++) {
        printk_dup("    base = %x, len = %x\n", (u32)phys_free_map[i].addr, (u32)phys_free_map[i].length);
        uintptr_t left_addr = phys_free_map[i].addr;
        uintptr_t right_addr = left_addr + phys_free_map[i].length - 1;
        if (left_addr <= KERNEL_START_PHYS && KERNEL_END_PHYS <= right_addr) {
            if (right_addr - PAGE_ALIGN_UP(KERNEL_END_PHYS) + 1 >= mem_total / 8 ) { // mem_total/8 is phys mm bitmap size
                bitmap_addr = (u64*)PAGE_ALIGN_UP(KERNEL_END_PHYS);
                break;
            } else {
                printk_dup("Cannot place phys bitmap after kernel. Halting...\n");
                while (1) {}
            }
        }
    }
    printk_dup("\n");

    physmem_init(bitmap_addr, phys_free_map, phys_free_map_len, mem_total);

    generic_main();

    #undef PREF
}

