#include <kernel/types.h>
#include <kernel/libk/assert.h>
#include <kernel/multiboot.h>
#include <kernel/io/kterm.h>
#include "../vgatext.h"

extern void gdt_install(void);
extern void idt_install(void);

void kmain(struct multiboot* mboot, u32 mboot_magic) {
    vgatext_init((void *)0xB8000);
    kterm_set_putchar_callback(vgatext_putchar);
    assert(mboot_magic == MULTIBOOT_EAX_MAGIC);
    (void)mboot;

    printk("  Hello from i686 kmain\n");

    gdt_install();
    printk("    GDT installed\n");

    idt_install();
    printk("    IDT installed\n");

    while (1) {}

    // generic_main();
}