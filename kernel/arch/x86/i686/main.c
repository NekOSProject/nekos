#include <kernel/types.h>

#include "../vgatext.h"
#include "../debug.h"
#include <kernel/libk/assert.h>
#include <kernel/multiboot.h>
#include <kernel/io/fdo.h>
#include <kernel/mm/memlayout.h>

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

    while (1) {}

    // generic_main();
    #undef PREF
}

