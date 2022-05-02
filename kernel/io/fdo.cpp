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

char buffer[512];

void fdo_set_putchar_screen_callback(void (*p)(char)) {
    putchar_screen_callback = p;
}

void fdo_set_putchar_log_callback(void (*p)(char)) {
    putchar_log_callback = p;
}

void fdo_set_default_color() {
    #if defined ARCH_i686
    vgatext_setcolor(vgatext_entry_color(termcolors::LIGHT_GREY, termcolors::BLACK));
    #else
    #error Error
    #endif
}

void fdo_set_text_color(termcolors text_color) {
     #if defined ARCH_i686
    vgatext_set_text_color((u8)text_color);
    #else
    #error Error
    #endif
}

void fdo_set_bg_color(termcolors bg_color) {
     #if defined ARCH_i686
    vgatext_set_bg_color((u8)bg_color);
    #else
    #error Error
    #endif
}

void fdo_putstring(void (*pc)(char), const char* str) {
    int state = 0, num = 0;
    while (*str) {

        if (state == 0) {
            if (*str == '\x1b') {
                state = 1;
            } else {
                pc(*str);
            }
        } else if (state == 1) {
            if (*str == '[') {
                state = 2;
            } else {
                pc(*str);
            }
        } else if (state == 2) {
            if (isdigit(*str)) {
                num = num*10 + (*str - '0');
            } else if (*str == ';' || *str == 'm') {
                switch (num) {
                    case 0: fdo_set_default_color(); break;
                    case 30: fdo_set_text_color(termcolors::BLACK); break;
                    case 31: fdo_set_text_color(termcolors::LIGHT_RED); break;
                    case 32: fdo_set_text_color(termcolors::LIGHT_GREEN); break;
                    case 33: fdo_set_text_color(termcolors::LIGHT_YELLOW); break;
                    case 34: fdo_set_text_color(termcolors::LIGHT_BLUE); break;
                    case 35: fdo_set_text_color(termcolors::LIGHT_MAGENTA); break;
                    case 36: fdo_set_text_color(termcolors::LIGHT_CYAN); break;
                    case 37: fdo_set_text_color(termcolors::WHITE); break;
                    case 40: fdo_set_bg_color(termcolors::BLACK); break;
                    case 41: fdo_set_bg_color(termcolors::LIGHT_RED); break;
                    case 42: fdo_set_bg_color(termcolors::LIGHT_GREEN); break;
                    case 43: fdo_set_bg_color(termcolors::LIGHT_YELLOW); break;
                    case 44: fdo_set_bg_color(termcolors::LIGHT_BLUE); break;
                    case 45: fdo_set_bg_color(termcolors::LIGHT_MAGENTA); break;
                    case 46: fdo_set_bg_color(termcolors::LIGHT_CYAN); break;
                    case 47: fdo_set_bg_color(termcolors::WHITE); break;
                }
                num = 0;
                if (*str == 'm') {
                    state = 0;
                }
            }
        }
        str++;
    }
}

void fdo_print(void (*pc)(char), const char *format, va_list args) {
    if (pc == NULL) return;
    vsprintf(buffer, format, args);
    fdo_putstring(pc, buffer);
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
