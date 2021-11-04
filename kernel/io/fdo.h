#pragma once

#include <stdarg.h>
#include <kernel/types.h>
#include <kernel/libk/string.h>

void printk_dup(char *fmt, ... );
void printk_screen(char *fmt, ... );
void printk_log(char *fmt, ... );

void fdo_set_putchar_log_callback(void (*p)(char));
void fdo_set_putchar_screen_callback(void (*p)(char));


