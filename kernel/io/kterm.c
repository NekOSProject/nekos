#include "kterm.h"

#if defined KERNEL_ARCH_IS_i686 || defined KERNEL_ARCH_IS_x86_64
#include <kernel/arch/x86/vgatext.h>
#define kterm_putchar vgatext_putchar
#else
#error "Unknown CPU architecture specified"
#endif

void kterm_putstring(const char* data) {
    for (int i = 0; i < strlen(data); i++) {
        kterm_putchar(data[i]);
    }
}

void kterm_putuint(int i) {
    u32 n, d = 1000000000;
    char str[255];
    u32 dec_index = 0;
    while ( ( i/d == 0 ) && ( d >= 10 ) ) d /= 10;
    n = i;
    while (d >= 10) {
        str[dec_index++] = ((char)((int)'0' + n/d));
        n = n % d;
        d /= 10;
    }
    str[dec_index++] = ((char)((int)'0' + n));
    str[dec_index] = 0;
    kterm_putstring(str);
}

void kterm_putint(int i) {
    if(i >= 0) {
        kterm_putuint(i);
    } else {
        kterm_putchar('-');
        kterm_putuint(-i);
    }
}

void kterm_puthex(u32 i) {
    const u8 hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    u32 n, d = 0x10000000;
    kterm_putstring("0x");
    while((i / d == 0) && (d >= 0x10)) d /= 0x10;
    n = i;
    while( d >= 0xF ) {
        kterm_putchar(hex[n/d]);
        n = n % d;
        d /= 0x10;
    }
    kterm_putchar(hex[n]);
}


void kterm_print(char *format, va_list args) {
    int i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                kterm_putstring(string);
                break;
            case 'c':
                // To-Do: fix this! "warning: cast to pointer from integer of different size"
                kterm_putchar(va_arg(args, int));
                break;
            case 'd':
                kterm_putint(va_arg(args, int));
                break;
            case 'i':
                kterm_putint(va_arg(args, int));
                break;
            case 'u':
                kterm_putuint(va_arg(args, unsigned int));
                break;
            case 'x':
                kterm_puthex(va_arg(args, u32));
                break;
            default:
                kterm_putchar(format[i]);
            }
        } else {
            kterm_putchar(format[i]);
        }
        i++;
    }
}

void printk(char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    kterm_print(fmt, args);
}