#include "kheap.h"
#include "../../kernel.h"
#include "heap.h"
#include <stddef.h>

struct heap kernel_heap;

struct heap_table kernel_heap_table;

void kheap_init()
{
    int total_table_entries = ZVIROS_HEAP_SIZE_BYTES / ZVIROS_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)ZVIROS_HEAP_TABLE_ADDERESS; // TODO
    kernel_heap_table.total = total_table_entries;

    void* end = (void*)(ZVIROS_HEAP_ADDRESS + ZVIROS_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*)ZVIROS_HEAP_ADDRESS, end, &kernel_heap_table);
    if (res < 0)
    {
        print("Failed to crete heap\n"); // TODO Should be panic. Implement later
    }
}

void* kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}
