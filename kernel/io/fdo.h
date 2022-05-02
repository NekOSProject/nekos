#pragma once

#include <stdarg.h>
#include <kernel/types.h>
#include <kernel/libk/string.h>
#include <kernel/libk/sprintf.h>
#include "termcolors.h"

#define FDO_KERNEL "[KERNEL]: "
#define FDO_ARCH_i686 "[ARCH i686]: "
#define FDO_PHYSMEM "[PMM]: "


void printk_dup(const char *fmt, ... );
void printk_screen(const char *fmt, ... );
void printk_log(const char *fmt, ... );

void fdo_set_putchar_log_callback(void (*p)(char));
void fdo_set_putchar_screen_callback(void (*p)(char));


