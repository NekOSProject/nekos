// for interrupts with no error code
.macro interrupt_handler_noerrorcode num
.global interrupt_handler\num
interrupt_handler\num:
    cli
    push    $0 //  push dummy error code
    push    $\num // push interrupt number
    jmp     common_interrupt_handler_wrapper
.endm

// for interrupts that produce error code
.macro interrupt_handler_errorcode num
.global interrupt_handler\num
interrupt_handler\num:
    cli
    push    $\num // push interrupt number
    jmp     common_interrupt_handler_wrapper
.endm

interrupt_handler_noerrorcode 0
interrupt_handler_noerrorcode 1
interrupt_handler_noerrorcode 2
interrupt_handler_noerrorcode 3
interrupt_handler_noerrorcode 4
interrupt_handler_noerrorcode 5
interrupt_handler_noerrorcode 6
interrupt_handler_noerrorcode 7
interrupt_handler_errorcode   8
interrupt_handler_noerrorcode 9
interrupt_handler_errorcode   10
interrupt_handler_errorcode   11
interrupt_handler_errorcode   12
interrupt_handler_errorcode   13
interrupt_handler_errorcode   14
interrupt_handler_noerrorcode 15
interrupt_handler_noerrorcode 16
interrupt_handler_noerrorcode 17
interrupt_handler_noerrorcode 18
interrupt_handler_noerrorcode 19
interrupt_handler_noerrorcode 20
interrupt_handler_noerrorcode 21
interrupt_handler_noerrorcode 22
interrupt_handler_noerrorcode 23
interrupt_handler_noerrorcode 24
interrupt_handler_noerrorcode 25
interrupt_handler_noerrorcode 26
interrupt_handler_noerrorcode 27
interrupt_handler_noerrorcode 28
interrupt_handler_noerrorcode 29
interrupt_handler_noerrorcode 30
interrupt_handler_noerrorcode 31

interrupt_handler_noerrorcode 32
interrupt_handler_noerrorcode 33
interrupt_handler_noerrorcode 34
interrupt_handler_noerrorcode 35
interrupt_handler_noerrorcode 36
interrupt_handler_noerrorcode 37
interrupt_handler_noerrorcode 38
interrupt_handler_noerrorcode 39
interrupt_handler_noerrorcode 40
interrupt_handler_noerrorcode 41
interrupt_handler_noerrorcode 42
interrupt_handler_noerrorcode 43
interrupt_handler_noerrorcode 44
interrupt_handler_noerrorcode 45
interrupt_handler_noerrorcode 46
interrupt_handler_noerrorcode 47

.extern common_interrupt_handler
common_interrupt_handler_wrapper:
    pusha   // save general purpose registers
    
    // save current DS in stack
    mov     %ds, %ax
    push    %eax

    // load kernel DS
    mov     $0x10, %ax // NOTE: why 0x10?
    mov     %ax, %ds
     // NOTE: why need this three lines
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs


    call    common_interrupt_handler

    // restore previous DS
    pop     %eax
    mov     %ax, %ds
    // NOTE: why need this three lines
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    popa    // restore general purpose registers

    // pop error code and interrupt number
    add     $8, %esp

    sti     // allow interrupts
    // return from interrupt handler
    // and pop CS, EIP, EFLAGS, SS, ESP
    iret    


