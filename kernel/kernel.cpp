/*
    NekOS
    Generic main kernel code
*/
#include "kernel.h"

namespace kernel {

int kmain() {
    printk_dup(FDO_KERNEL "Hello from kmain()\n");

    physmem::test();

    printk_dup(FDO_KERNEL "%wTest%y %u %wcolors%y %u\n", termcolors::LIGHT_GREEN, 1337, termcolors::LIGHT_CYAN, 356253);

    while (1) {}
    
    return 0;
}

};