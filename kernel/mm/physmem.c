/*
    NekOS
    Physical memory management unit
*/
#include "physmem.h"

// bitmap bitness
// how many bits stores one element of bitmap
#define BB 64

#define PREF "[PMM]: "

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

void physmem_init(uintptr_t bitmap_addr, physmem_free_map_entry_t free_map[], size_t free_map_length, size_t mem_total) {
    page_count = mem_total / PAGE_SIZE;
    used_page_count = page_count;
    first_free_page = (size_t)-1;
    last_free_page = (size_t)-1;
    bitmap = (u64 *)bitmap_addr;
    size_t bitmap_size = page_count / 8;
    memset(bitmap, 0xFF, bitmap_size); // mark all pages as used
    phys_bitmap_start = (uintptr_t)bitmap;
    phys_bitmap_end = phys_bitmap_start + bitmap_size;
    printk_dup(PREF "Physical memory bitmap placed in [%x:%x]\n", phys_bitmap_start, phys_bitmap_end);
    for (size_t i = 0; i < free_map_length; i++) {
        // TODO: physmem_free_pages(PAGE_ALIGN_UP(free_map[i].addr), free_map[i].length / PAGE_SIZE);
    }
    // TODO: mark kernel memory as used
    // TODO: mark bitmap memory as used
}

#undef PREF
