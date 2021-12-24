#include "fdo.h"

#if defined ARCH_i686
#include <kernel/arch/x86/vgatext.h>
#include <kernel/arch/x86/debug.h>
void (*putchar_screen_callback)(char) = vgatext_putchar;
void (*putchar_log_callback)(char) = debug_putchar;
#else
void (*putchar_screen_callback)(char) = 0;
void (*putchar_log_callback)(char) = 0;
#endif

void fdo_set_putchar_screen_callback(void (*p)(char)) {
    putchar_screen_callback = p;
}

void fdo_set_putchar_log_callback(void (*p)(char)) {
    putchar_log_callback = p;
}

void fdo_putstring(void (*pc)(char), const char* str) {
    while (*str) {
        pc(*str);
        str++;
    }
}

void fdo_putuint(void (*pc)(char), int i) {
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
    fdo_putstring(pc, str);
}

void fdo_putint(void (*pc)(char), int i) {
    if(i >= 0) {
        fdo_putuint(pc, i);
    } else {
        pc('-');
        fdo_putuint(pc, -i);
    }
}

void fdo_puthex(void (*pc)(char), u32 i) {
    const u8 hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    u32 n, d = 0x10000000;
    fdo_putstring(pc, "0x");
    while((i / d == 0) && (d >= 0x10)) d /= 0x10;
    n = i;
    while( d >= 0xF ) {
        pc(hex[n/d]);
        n = n % d;
        d /= 0x10;
    }
    pc(hex[n]);
}

void fdo_print(void (*pc)(char), const char *format, va_list args) {
    if (pc == NULL) return;
    int i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                fdo_putstring(pc, string);
                break;
            case 'c':
                pc(va_arg(args, int));
                break;
            case 'd':
                fdo_putint(pc, va_arg(args, int));
                break;
            case 'i':
                fdo_putint(pc, va_arg(args, int));
                break;
            case 'u':
                fdo_putuint(pc, va_arg(args, unsigned int));
                break;
            case 'x':
                fdo_puthex(pc, va_arg(args, u32));
                break;
            default:
                pc(format[i]);
            }
        } else {
            pc(format[i]);
        }
        i++;
    }
}

void printk_dup(const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    fdo_print(putchar_log_callback, fmt, args);
    fdo_print(putchar_screen_callback, fmt, args);
}

void printk_screen(const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    fdo_print(putchar_screen_callback, fmt, args);
}

void printk_log(const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    fdo_print(putchar_log_callback, fmt, args);
}
