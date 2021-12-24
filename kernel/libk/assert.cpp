#include "assert.h"

void __assert_func(const char * file, int line, const char * func, const char * failedexpr) {
    printk_dup("Assertion failed in %s:%d (%s): %s\n", file, line, func, failedexpr);
}