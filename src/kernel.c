#include "kernel.h"
#include "./idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include <stddef.h>
#include <stdint.h>

uint16_t terminal_x = 0;
uint16_t terminal_y = 0;
uint16_t* video_mem = 0;

size_t strlen(const char* str)
{
    size_t index = 0;
    while (str[index] != 0)
    {
        index++;
    }

    return index;
}
uint16_t make_char(char c, char color)
{
    return (color << 8) + c;
}

void terminal_put_char(uint16_t x, uint16_t y, char c, char color)
{
    video_mem[(y * VGA_WIDTH + x)] = make_char(c, color);
}

void write_char(char c, char color)
{
    if (c == '\n')
    {
        terminal_x = 0;
        terminal_y++;
        return;
    }
    terminal_put_char(terminal_x, terminal_y, c, color);
    terminal_x++;
    if (terminal_x >= VGA_WIDTH)
    {
        terminal_x = 0;
        terminal_y++;
    }
}

void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; ++i)
    {
        write_char(str[i], 1);
    }
}

void init_screen()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_x = 0;
    terminal_y = 0;
    for (int i = 0; i < VGA_WIDTH * VGA_WIDTH; ++i)
    {
        video_mem[i] = make_char(' ', 0);
    }
}
struct paging_4gb_chunk* kernel_chunk = 0;
void kernel_main()
{
    init_screen();
    kheap_init();
    idt_init();
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    char* ptr = kzalloc(4096);
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000,
               (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITEABLE);

    enable_paging();
    char* ptr2 = (char*) 0x1000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    print(ptr2);
    print(ptr);
    enable_interupts();
}
