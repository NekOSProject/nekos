/*
    NekOS
    Physical memory management unit
*/
#include "physmem.h"

// bitmap bitness
// how many bits stores one element of bitmap
#define BB 8

#define PREF "[PMM]: "

static u8* bitmap;
static size_t page_count;
static size_t used_page_count;
static size_t first_free_page; // index of first free phys page
static size_t last_free_page;  // index of last free phys page

uintptr_t phys_bitmap_start;
uintptr_t phys_bitmap_end;

inline static void bitmap_set(size_t bit) {
    bitmap[bit / BB] |= (1 << (bit % BB));
}

inline static void bitmap_unset(size_t bit) {
    bitmap[bit / BB] &= ~(1 << (bit % BB));
}

inline static bool bitmap_test(size_t bit) {
    return bitmap[bit / BB] & (1 << (bit % BB));
}

// Free n pages starting from address addr
void physmem_free_pages(uintptr_t addr, size_t n) {
    size_t first_page_idx = addr / PAGE_SIZE;
    for (size_t i = 0; i < n; i++) {
        if (bitmap_test(first_page_idx + i)) {
            bitmap_unset(first_page_idx + i);
            used_page_count--;
        }
    }
}

// Mark as used n pages starting from address addr
void physmem_mark_pages_as_used(uintptr_t addr, size_t n) {
    size_t first_page_idx = addr / PAGE_SIZE;
    for (size_t i = 0; i < n; i++) {
        if (!bitmap_test(first_page_idx + i)) {
            bitmap_set(first_page_idx + i);
            used_page_count++;
        }
    }
}

void physmem_init(uintptr_t bitmap_addr, physmem_free_map_entry_t free_map[], size_t free_map_length, size_t mem_total) {
    page_count = mem_total / PAGE_SIZE;
    printk_dup(PREF "mem_total = %u bytes\n", mem_total);
    used_page_count = page_count;
    printk_dup(PREF "used_page_count = %u\n", used_page_count);
    first_free_page = (size_t)-1;
    last_free_page = (size_t)-1;
    bitmap = (u8*)bitmap_addr;
    size_t bitmap_size = page_count / 8;
    memset(bitmap, 0xFF, bitmap_size); // mark all pages as used
    phys_bitmap_start = (uintptr_t)bitmap;
    phys_bitmap_end = phys_bitmap_start + bitmap_size;
    printk_dup(PREF "Physical memory bitmap placed in [%x:%x]\n", phys_bitmap_start, phys_bitmap_end);
    for (size_t i = 0; i < free_map_length; i++) {
        physmem_free_pages(PAGE_ALIGN_UP(free_map[i].addr), free_map[i].length / PAGE_SIZE);
        printk_dup(PREF "Freed addr = %x, length in pages = %u\n", free_map[i].addr, free_map[i].length / PAGE_SIZE);
    }
    printk_dup(PREF "used_page_count = %u\n", used_page_count);

    physmem_mark_pages_as_used(KERNEL_START_PHYS, KERNEL_SIZE / PAGE_SIZE);
    physmem_mark_pages_as_used(phys_bitmap_start, bitmap_size / PAGE_SIZE);
    printk_dup(PREF "used_page_count = %u\n", used_page_count);
}

#undef PREF
