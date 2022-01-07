/*
    NekOS
    Generic main kernel code
*/
#include "kernel.h"

namespace kernel {

int kmain() {
    printk_dup(FDO_KERNEL "%wHello from kmain()%y\n", termcolors::LIGHT_GREEN);

    physmem::test();

    while (1) {}
    
    return 0;
}

};