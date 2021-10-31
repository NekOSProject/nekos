#include <kernel/types.h>
#include <kernel/multiboot.h>
#include <kernel/io/kterm.h>

void kmain(struct multiboot* mboot, u32 mboot_magic) {
    


    assert(mboot_magic == MULTIBOOT_EAX_MAGIC);

    gdt_install();      // Global descriptor table
	idt_install();      // IDT

    // generic_main();
}