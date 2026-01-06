#include "heap.h"
#include "../../kernel.h"
#include "../../status.h"
#include "../memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static int validate_table(void* ptr, void* end, struct heap_table* heap_table)
{
    size_t table_size = (size_t)(end - ptr);
    size_t total_blocks = table_size / ZVIROS_HEAP_BLOCK_SIZE;
    if (heap_table->total != total_blocks)
    {
        return -EINVARG;
    }

    return 0;
}
static bool validate_alligment(void* ptr)
{
    return ((size_t)ptr % ZVIROS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table)
{
    int res = 0;
    if (!validate_alligment(ptr) || !validate_alligment(end))
    {
        res = -EINVARG;
        goto out; // EVIL GOTO :) linux is doing things sometimes similar to this. So it can do
                  // cleanup before return. I understand this as mimicing defer in golang
    }
    memoryset(heap, 0, sizeof(struct heap));
    heap->start_address = ptr;
    heap->table = table;
    res = validate_table(ptr, end, table);
    if (res < 0)
    {
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memoryset(table->entries, HEAP_BLOCK_FREE, table_size);

out:
    // do cleanup if needed like defer in golang
    return res;
}

static uint32_t heap_allign_value_to_upper(uint32_t val)
{
    uint32_t remainder = val % ZVIROS_HEAP_BLOCK_SIZE;
    if (!remainder)
    {
        return val;
    }

    return (val - remainder) + ZVIROS_HEAP_BLOCK_SIZE;
}

void* heap_block_to_start_address(struct heap* heap, size_t start_block)
{
    return (void*)(start_block * ZVIROS_HEAP_BLOCK_SIZE + heap->start_address);
}

int get_block_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0xF;
}

int heap_get_start_block(struct heap* heap, int total_blocks)
{
    struct heap_table* table = heap->table;
    int bc = 0;
    int bs = -1;

    for (size_t i = 0; i < table->total; ++i)
    {
        if (get_block_type(table->entries[i]) != HEAP_BLOCK_FREE)
        {
            bc = 0;
            bs = -1;
            continue;
        }

        if (bs == -1)
        {
            bs = i;
        }
        bc++;
        if (bc == total_blocks)
        {
            break;
        }
    }

    if (bs == -1)
    {
        return -ENOMEM;
    }

    return bs;
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, uint32_t total_blocks)
{
    for (size_t i = start_block; i < start_block + total_blocks; ++i)
    {
        heap->table->entries[i] = HEAP_BLOCK_TAKEN;
        if (i < start_block + total_blocks - 1)
        {

            heap->table->entries[i] |= HEAP_BLOCK_HAS_NEXT;
        }
    }

    heap->table->entries[start_block] |= HEAP_BLOCK_IS_FIRST;
}

void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks)
{
    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0)
    {
        return 0;
    }
    heap_mark_blocks_taken(heap, start_block, total_blocks);
    return heap_block_to_start_address(heap, start_block);
}

void* heap_malloc(struct heap* heap, int size)
{
    uint32_t alligned_size = heap_allign_value_to_upper(size);
    uint32_t total_blocks = alligned_size / ZVIROS_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap* heap, void* ptr)
{
    size_t start_block = ((size_t)ptr - (size_t)heap->start_address) / ZVIROS_HEAP_BLOCK_SIZE;
    int i = start_block;
    while (heap->table->entries[i] & HEAP_BLOCK_TAKEN)
    {
        bool has_next = heap->table->entries[i] & HEAP_BLOCK_HAS_NEXT;
        heap->table->entries[i] = HEAP_BLOCK_FREE;
        if (!has_next)
        {
            break;
        }
    }
}
