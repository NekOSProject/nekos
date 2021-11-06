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

    // parse memory map from grub
    mboot_memmap_t *memmap = mboot->mmap_addr;
    int memmap_entries_count = mboot->mmap_length / sizeof(mboot_memmap_t);
    physmem_free_map_entry_t phys_free_map[memmap_entries_count];
    int phys_free_map_len = 0;
    for (int i = 0; i < memmap_entries_count; i++) {
        if (memmap[i].type = MULTIBOOT_MEMORY_AVAILABLE) {
            phys_free_map[phys_free_map_len++] = (physmem_free_map_entry_t){memmap[i].base_addr, memmap[i].length};
        }
    }

    printk_dup(PREF "phys free map:\n");
    for (int i = 0; i < phys_free_map_len; i++) {
        printk_dup("    base = %x, len = %x\n", (u32)phys_free_map[i].addr, (u32)phys_free_map[i].length);
    }
    printk_dup("\n");

    // TODO: physmem_init(phys_free_map) !

    generic_main();

    #undef PREF
}

