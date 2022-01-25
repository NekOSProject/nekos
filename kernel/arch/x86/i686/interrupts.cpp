#include "interrupts.h"
#include "../pic.h"
#include <kernel/io/fdo.h>

interrupt_handler_t interrupt_handler_table[256];

void set_interrupt_handler(u8 num, interrupt_handler_t handler) {
    interrupt_handler_table[num] = handler;
}

extern "C" void common_interrupt_handler(interrupt_ctx_t regs) {
    if (interrupt_handler_table[regs.int_num]) {
        interrupt_handler_table[regs.int_num](regs);
    }
    // if interrupts goes through PIC we must send End Of Interrupt (EOI) to PIC
    if (regs.int_num >= 32) {
        // if interrupt came from slave PIC, send EOI to it
        if (regs.int_num >= 40) {
            outb(0xA0, 0x20);
        }
        // send EOI to master PIC enyway
        outb(0x20, 0x20);
    }
}

void divide_by_zero(interrupt_ctx_t regs) {
    (void)regs;
    printk_dup(FDO_ARCH_i686 "%wDivide by zero!%y", termcolors::LIGHT_RED);
    asm("hlt");
}

