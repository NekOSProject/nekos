#include "pic.h"

void pic_mask_all() {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void pic_unmask_all() {
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

// Performs PIC remapping
// Offset of controller is a number,
// that controller adds to irq line number before passing it to the cpu.
// Offset must be divisible by 8 because its first 3 bits stand for irq line number.
// We set master offset to 0x20 (32), slave offset to 0x28 (40),
// this done because vectors 0..31 used for cpu exceptions, and 0x20 is 32.
// After that master will give interrupt vectors 32..39, and slave will give 40..47.
// Interrupt vector 34 cannot happen because irq2 is connected with slave controller.
// So, we can receive only 15 external interrupts.
// Its not much, but we also use software interrupts (int instruction),
// because they go directly to the cpu, bypassing pic.
void pic_remap() {
    // 0x20 - master pic command port
    // 0x21 - master pic data port
    // 0xA0 - slave pic command port
    // 0xA1 - slave pic data port
    outb(0x20, 0x11);// starts the initialization sequence (in cascade mode)
    outb(0xA0, 0x11);
    io_wait();
    outb(0x21, 0x20); // ICW2: Master PIC vector offset
    outb(0xA1, 0x28); // ICW2: Slave PIC vector offset
    io_wait();
    outb(0x21, 0x04); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();
    outb(0x21, 0x01); // 8086/88 (MCS-80/85) mode
    outb(0xA1, 0x01);
    io_wait();
}