#pragma once

#include <kernel/types.h>
#include "ports.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_W_x_H 2000
#define VGA_TEXT_MEMORY_PHYS 0xB8000

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
