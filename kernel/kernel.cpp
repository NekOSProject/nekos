/*
    NekOS
    Generic main kernel code
*/
#include "kernel.h"

namespace kernel {

int kmain() {
    printk_dup(FDO_KERNEL "Hello from kmain()\n");

    physmem::test();

    while (1) {}
    
    return 0;
}

};