#pragma once

#include <kernel/types.h>
#include "ports.h"

static constexpr u32 VGA_WIDTH =  80;
static constexpr u32 VGA_HEIGHT = 25;

// VGA colors
enum vgatext_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};


void vgatext_putchar(char c);

void vgatext_init(void *_vgatext_base);
void vgatext_backspace();
void vgatext_setcolor(u8 color);
void vgatext_scroll();
void vgatext_write(const char* data, u32 size);
void vgatext_update_cursor(u32 row, u32 col);
void vgatext_putentryat(char c, u8 color, u32 x, u32 y);

