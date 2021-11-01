#pragma once

#include <stdarg.h>
#include <kernel/types.h>
#include <kernel/libk/string.h>

void printk(char *fmt, ... );

void kterm_set_putchar_callback(void (*p)(char));


