/*
    NekOS
    Generic main kernel code
*/
#include "kernel.h"

namespace kernel {

int kmain() {

    volatile int x = 0;
    volatile int y = 5 / x;
    (void)x;
    (void)y;

    printk_dup(FDO_KERNEL "%wHello from kmain()%y\n", termcolors::LIGHT_GREEN);

    physmem::test();

    // infinite loop. volatile preserves from optimizing out.
    volatile int a = 1;
    (void)a;
    while (1) {
        a = 0;
    }
    
    return 0;
}

};