#include "debug.h"
#include "ports.h"

void debug_putchar(char c) {
    outb(0xe9, c);
}

