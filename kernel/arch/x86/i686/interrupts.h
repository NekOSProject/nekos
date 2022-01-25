#include <kernel/types.h>

typedef struct {
  u32  ds;
  u32  edi, esi, ebp, esp, ebx, edx, ecx, eax;
  u32  int_num, err_code;
  u32  eip, cs, eflags, useresp, ss;
} __attribute__((packed)) interrupt_ctx_t;

using interrupt_handler_t = void (*)(interrupt_ctx_t);

void set_interrupt_handler(u8 num, interrupt_handler_t handler);
extern "C" void common_interrupt_handler(interrupt_ctx_t regs);
void divide_by_zero(interrupt_ctx_t regs);


// interrupt handlers declarations:
extern "C" void interrupt_handler0();
extern "C" void interrupt_handler1();
extern "C" void interrupt_handler2();
extern "C" void interrupt_handler3();
extern "C" void interrupt_handler4();
extern "C" void interrupt_handler5();
extern "C" void interrupt_handler6();
extern "C" void interrupt_handler7();
extern "C" void interrupt_handler8();
extern "C" void interrupt_handler9();
extern "C" void interrupt_handler10();
extern "C" void interrupt_handler11();
extern "C" void interrupt_handler12();
extern "C" void interrupt_handler13();
extern "C" void interrupt_handler14();
extern "C" void interrupt_handler15();
extern "C" void interrupt_handler16();
extern "C" void interrupt_handler17();
extern "C" void interrupt_handler18();
extern "C" void interrupt_handler19();
extern "C" void interrupt_handler20();
extern "C" void interrupt_handler21();
extern "C" void interrupt_handler22();
extern "C" void interrupt_handler23();
extern "C" void interrupt_handler24();
extern "C" void interrupt_handler25();
extern "C" void interrupt_handler26();
extern "C" void interrupt_handler27();
extern "C" void interrupt_handler28();
extern "C" void interrupt_handler29();
extern "C" void interrupt_handler30();
extern "C" void interrupt_handler31();

// extern "C" void interrupt_handler32();
// extern "C" void interrupt_handler33();
// extern "C" void interrupt_handler34();
// extern "C" void interrupt_handler35();
// extern "C" void interrupt_handler36();
// extern "C" void interrupt_handler37();
// extern "C" void interrupt_handler38();
// extern "C" void interrupt_handler39();
// extern "C" void interrupt_handler40();
// extern "C" void interrupt_handler41();
// extern "C" void interrupt_handler42();
// extern "C" void interrupt_handler43();
// extern "C" void interrupt_handler44();
// extern "C" void interrupt_handler45();
// extern "C" void interrupt_handler46();
// extern "C" void interrupt_handler47();


