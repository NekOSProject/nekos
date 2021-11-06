/*
    NekOS
    Physical memory management unit
*/
#include "physmem.h"

// bitmap bitness
// how many bits stores one element of bitmap
#define BB 64

static u64* bitmap;
static size_t page_count;
static size_t used_page_count;
static size_t first_free_page; // index of first free phys page
static size_t last_free_page;  // index of last free phys page

uintptr_t phys_bitmap_start;
uintptr_t phys_bitmap_end;

inline static void bitmap_set(int bit) {
    bitmap[bit / BB] |= (1 << (bit % BB));
}

inline static void bitmap_unset(int bit) {
    bitmap[bit / BB] &= ~(1 << (bit % BB));
}

inline static bool bitmap_test(int bit) {
    return bitmap[bit / BB] & (1 << (bit % BB));
}

// TODO: physmem_init, etc.
