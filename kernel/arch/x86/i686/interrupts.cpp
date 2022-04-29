#include "interrupts.h"
#include "../pic.h"
#include <kernel/io/fdo.h>

interrupt_handler_t interrupt_handler_table[256];

void set_interrupt_handler(u8 num, interrupt_handler_t handler) {
    interrupt_handler_table[num] = handler;
}

extern "C" void common_interrupt_handler(interrupt_ctx_t regs) {
    // printk_dup(FDO_ARCH_i686 "Interrupt %u occured.\n", regs.int_num);
    // if interrupts goes through PIC we must send End Of Interrupt (EOI) to PIC
    if (regs.int_num >= 32) {
        // if interrupt came from slave PIC, send EOI to it
        if (regs.int_num >= 40) {
            outb(0xA0, 0x20);
        }
        // send EOI to master PIC enyway
        outb(0x20, 0x20);
    }

    // Note: we run interrupt handler after sending EOI
    // because some devices like PS/2 keyboard require to do so:
    // send EOI before handling interrupt

    if (interrupt_handler_table[regs.int_num]) {
        interrupt_handler_table[regs.int_num](regs);
    }
}

void divide_by_zero(interrupt_ctx_t regs) {
    (void)regs;
    printk_dup(FDO_ARCH_i686 "%wDivide by zero!%y", termcolors::LIGHT_RED);
    asm("hlt");
}

void fault_handler(interrupt_ctx_t regs) {
    uint32_t adr;
    asm volatile("movl %%cr2, %0" : "=r" (adr));
    printk_dup(FDO_ARCH_i686 "System Halted! cr2 = %x  int_num = %x, err_code = %x, eax = %x  ebx = %x  ecx = %x  edx = %x  esp = %x  ebp = %x  eip = %x\n", adr, regs.int_num, regs.err_code, regs.eax, regs.ebx, regs.ecx, regs.edx, regs.esp, regs.ebp, regs.eip);
    asm volatile("hlt");
}

