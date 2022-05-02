/*
    NekOS
    Generic main kernel code
*/
#include "kernel.h"
#include <kernel/libk/sprintf.h>
#include <kernel/libk/assert.h>

namespace kernel {

void test_sprintf() {
    char text[150];
    sprintf(text, "d = %d, v = 0x%x, val = %lld, val2 = %llu, s = %s, char = %c\n", -123, 0xABC, 9223372036854775807, 18446744073709551615ULL, "str", 'b');
    //printk_dup(FDO_KERNEL "%s", text);
    assert(strcmp(text, "d = -123, v = 0xabc, val = 9223372036854775807, val2 = 18446744073709551615, s = str, char = b\n") == 0);
}

int kmain() {

    // volatile int x = 0;
    // volatile int y = 5 / x;
    // (void)x;
    // (void)y;

    printk_dup(FDO_KERNEL "\033[0;32mHello from kmain()\033[0m\n");

    physmem::test();

    test_sprintf();

    
    // infinite loop. volatile preserves from optimizing out.
    volatile int a = 1;
    (void)a;
    while (1) {
        a = 0;
    }

    return 0;
}

};