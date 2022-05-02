/* SPDX-License-Identifier: BSD-2-Clause */
#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "ctype.h"
#include "cdefs.h"

int sprintf(char *s, const char *fmt, ...) __format(printf, 2, 3);
int vsprintf(char *s, const char *fmt, va_list va);
int snprintf(char *s, size_t n, const char *fmt, ...) __format(printf, 3, 4);
int vsnprintf(char *s, size_t n, const char *fmt, va_list va);
