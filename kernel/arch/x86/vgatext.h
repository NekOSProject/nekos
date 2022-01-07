#pragma once

#include <kernel/types.h>
#include <kernel/io/termcolors.h>
#include "ports.h"

static constexpr u32 VGA_WIDTH  = 80;
static constexpr u32 VGA_HEIGHT = 25;

static inline u8 vgatext_entry_color(termcolors fg, termcolors bg) {
    return (u8)fg | (u8)bg << 4;
}

static inline u16 vgatext_entry(unsigned char uc, u8 color) {
    return (u16) uc | (u16) color << 8;
}

void vgatext_putchar(char c);

void vgatext_init(void *_vgatext_base);
void vgatext_backspace();
void vgatext_setcolor(u8 color);
void vgatext_scroll();
void vgatext_write(const char* data, u32 size);
void vgatext_update_cursor(u32 row, u32 col);
void vgatext_putentryat(char c, u8 color, u32 x, u32 y);

