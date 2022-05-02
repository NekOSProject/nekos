#include "vgatext.h"

u16* vgatext_buffer;

u32 vgatext_line_fill[VGA_HEIGHT];
u32 vgatext_row;
u32 vgatext_column;
u8 vgatext_color;

bool vgatext_initialized = false;

void vgatext_init(void *_vgatext_base) {
    vgatext_row = 0;
    vgatext_column = 0;
    vgatext_color = vgatext_entry_color(termcolors::LIGHT_GREY, termcolors::BLACK);
    vgatext_buffer = (u16*)_vgatext_base;

    for (u32 y = 0; y < VGA_HEIGHT; y++) {
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            const u32 index = y * VGA_WIDTH + x;
            vgatext_buffer[index] = vgatext_entry(' ', vgatext_color);
        }
    }
    vgatext_initialized = true;
}

void vgatext_backspace() {
    if (vgatext_column == 0) {
        if (vgatext_row > 0) {
            vgatext_row--;
        }
        vgatext_column = vgatext_line_fill[vgatext_row];
    } else {
        vgatext_column--;
    }
    vgatext_putentryat(32, vgatext_color, vgatext_column, vgatext_row);
    vgatext_update_cursor(vgatext_row, vgatext_column);
}

void vgatext_setcolor(u8 color) {
    vgatext_color = color;
}

void vgatext_set_text_color(u8 color) {
    vgatext_color >>= 4;
    vgatext_color <<= 4;
    vgatext_color |= color;
}

void vgatext_set_bg_color(u8 color) {
    vgatext_color <<= 4;
    vgatext_color >>= 4;
    vgatext_color |= (color << 4);
}

void vgatext_putchar(char c) {
    if (c != '\n') {
        vgatext_putentryat(c, vgatext_color, vgatext_column, vgatext_row);
    }
    if (++vgatext_column == VGA_WIDTH || c == '\n') {
        vgatext_line_fill[vgatext_row] = vgatext_column - 1;
        vgatext_column = 0;
        if (++vgatext_row == VGA_HEIGHT) {
            vgatext_scroll();
        }
    }
    vgatext_update_cursor(vgatext_row, vgatext_column);
}

void vgatext_scroll() {
    vgatext_row--;
    for (u32 y = 0; y < VGA_HEIGHT - 1; y++) {
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            const u32 src_index = y * VGA_WIDTH + x;
            const u32 dsvgatext_index = (y + 1) * VGA_WIDTH + x;
            vgatext_buffer[src_index] = vgatext_buffer[dsvgatext_index];
        }
        vgatext_line_fill[y] = vgatext_line_fill[y + 1];
    }
    for (u32 x = 0; x < VGA_WIDTH; x++) {
        const u32 index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vgatext_buffer[index] = vgatext_entry(' ', vgatext_color);
    }
}

void vgatext_write(const char* data, u32 size) {
    for (u32 i = 0; i < size; i++)
        vgatext_putchar(data[i]);
}

void vgatext_update_cursor(u32 row, u32 col) {
    u16 position = (row * VGA_WIDTH) + col;
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(position & 0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((position >> 8) & 0xFF));
}

void vgatext_putentryat(char c, u8 color, u32 x, u32 y) {
    const u32 index = y * VGA_WIDTH + x;
    vgatext_buffer[index] = vgatext_entry(c, color);
}
