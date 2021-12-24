/*
    NekOS
    Physical memory management unit
*/
#include "physmem.h"

// bitmap bitness
// how many bits stores one element of bitmap
#define BB 8

namespace physmem {

static u8* bitmap;
static size_t page_count;
static size_t used_page_count;
static size_t start_search_free_page;   // start index when searching for free pages
static size_t end_search_free_page;     // end index when searching for free pages

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
void free_pages(uintptr_t addr, size_t n) {
    size_t first_page_idx = addr / PAGE_SIZE;
    if (first_page_idx < start_search_free_page) {
        start_search_free_page = first_page_idx;
    }
    if (first_page_idx > end_search_free_page) {
        end_search_free_page = first_page_idx + n - 1;
    }
    for (size_t i = 0; i < n; i++) {
        if (bitmap_test(first_page_idx + i)) {
            bitmap_unset(first_page_idx + i);
            used_page_count--;
        }
    }
}

// Mark as used n pages starting from address addr
void mark_pages_as_used(uintptr_t addr, size_t n) {
    size_t first_page_idx = addr / PAGE_SIZE;
    if (first_page_idx == start_search_free_page) {
        start_search_free_page += n;
    }
    if (first_page_idx == end_search_free_page - n + 1) {
        end_search_free_page = first_page_idx - 1;
    }
    for (size_t i = 0; i < n; i++) {
        if (!bitmap_test(first_page_idx + i)) {
            bitmap_set(first_page_idx + i);
            used_page_count++;
        }
    }
}

// search for N paged contignuous block
// in [start_search_free_page, end_search_free_page] range
uintptr_t alloc_pages(size_t n) {
    size_t idx = start_search_free_page;
    while (idx <= end_search_free_page) {
        // printk_dup("%u\n", idx);
        if (!bitmap_test(idx)) {
            size_t start_idx = idx;
            while (!bitmap_test(idx) && idx <= end_search_free_page) {
                idx++;
                if (idx - start_idx == n) {
                    mark_pages_as_used(start_idx*PAGE_SIZE, n);
                    return start_idx*PAGE_SIZE;
                }
            }
        } else {
            idx++;
        }
    }
    return 0;
}

void init(uintptr_t bitmap_addr, free_map_entry_t free_map[], size_t free_map_length, size_t mem_total) {
    page_count = mem_total / PAGE_SIZE;
    printk_dup(FDO_PHYSMEM "mem_total = %u bytes\n", mem_total);
    used_page_count = page_count;
    printk_dup(FDO_PHYSMEM "used_page_count = %u\n", used_page_count);
    bitmap = (u8*)bitmap_addr;
    size_t bitmap_size = page_count / 8;
    memset(bitmap, 0xFF, bitmap_size); // mark all pages as used
    phys_bitmap_start = (uintptr_t)bitmap;
    phys_bitmap_end = phys_bitmap_start + bitmap_size;
    printk_dup(FDO_PHYSMEM "Physical memory bitmap placed in [%x:%x]\n", phys_bitmap_start, phys_bitmap_end);
    // Now free available memory:
    bool search_bounds_undefined = true;    // true if start_search_free_page and end_search_free_page undefined
    for (size_t i = 0; i < free_map_length; i++) {
        uintptr_t addr_to_free = PAGE_ALIGN_UP(free_map[i].addr);
        size_t pages_to_free = free_map[i].length / PAGE_SIZE;
        if (search_bounds_undefined) {
            start_search_free_page = addr_to_free / PAGE_SIZE;
            end_search_free_page = page_count - 1;
            search_bounds_undefined = false;
        }    
        free_pages(addr_to_free, pages_to_free);
        printk_dup(FDO_PHYSMEM "Freed addr = %x, length in pages = %u\n", free_map[i].addr, free_map[i].length / PAGE_SIZE);
    }
    printk_dup(FDO_PHYSMEM "used_page_count = %u\n", used_page_count);

    mark_pages_as_used(KERNEL_START_PHYS, KERNEL_SIZE / PAGE_SIZE + (KERNEL_SIZE % PAGE_SIZE != 0));
    mark_pages_as_used(phys_bitmap_start, bitmap_size / PAGE_SIZE + (bitmap_size % PAGE_SIZE != 0));
    printk_dup(FDO_PHYSMEM "used_page_count = %u\n", used_page_count);
}

void test() {
    // TODO: remove (u32) . Now it used cuz printk cannot print 64 bit numbers
    printk_dup(FDO_PHYSMEM "Starting PMM test\n");
    printk_dup(FDO_PHYSMEM "start_search_free_page = %u end_search_free_page = %u\n", start_search_free_page, end_search_free_page);
    
    uintptr_t x = alloc_pages(1);
    printk_dup(FDO_PHYSMEM "x = physmem::alloc_pages(1) = %x\n", (u32)x);
    for (int i = 0; i < 4; i++) {
        printk_dup(FDO_PHYSMEM "physmem::alloc_pages(1) = %x\n", (u32)alloc_pages(1));
    }
    printk_dup(FDO_PHYSMEM "physmem::alloc_pages(2) = %x\n", (u32)alloc_pages(2));
    for (int i = 0; i < 3; i++) {
        printk_dup(FDO_PHYSMEM "physmem::alloc_pages(1) = %x\n", (u32)alloc_pages(1));
    }
    printk_dup(FDO_PHYSMEM "start_search_free_page = %u end_search_free_page = %u\n", start_search_free_page, end_search_free_page);
    printk_dup(FDO_PHYSMEM "physmem::free_pages(%x, 1)\n", (u32)x);
    free_pages(x, 1);
    printk_dup(FDO_PHYSMEM "start_search_free_page = %u end_search_free_page = %u\n", start_search_free_page, end_search_free_page);
    printk_dup(FDO_PHYSMEM "physmem::alloc_pages(1) = %x\n", (u32)alloc_pages(1));
    printk_dup(FDO_PHYSMEM "PMM test ended\n");
}

};
