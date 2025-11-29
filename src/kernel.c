#include "kernel.h"
#include <stddef.h>
#include <stdint.h>

uint16_t terminal_x = 0;
uint16_t terminal_y = 0;
uint16_t *video_mem = 0;

size_t strlen(const char *str) {
  size_t index = 0;
  while (str[index] != 0) {
    index++;
  }

  return index;
}
uint16_t make_char(char c, char color) { return (color << 8) + c; }

void terminal_put_char(uint16_t x, uint16_t y, char c, char color) {
  video_mem[(y * VGA_WIDTH + x)] = make_char(c, color);
}

void write_char(char c, char color) {
  terminal_put_char(terminal_x, terminal_y, c, color);
  terminal_x++;
  if (terminal_x >= VGA_WIDTH) {
    terminal_x = 0;
    terminal_y++;
  }
}

void print(const char *str) {
  size_t len = strlen(str);
  for (int i = 0; i < len; ++i) {
    write_char(str[i], 1);
  }
}

void init_screen() {
  video_mem = (uint16_t *)(0xB8000);
  terminal_x = 0;
  terminal_y = 0;
  for (int i = 0; i < VGA_WIDTH * VGA_WIDTH; ++i) {
    video_mem[i] = make_char(' ', 0);
  }
}

void kernel_main() {
  init_screen();
  print("Hello Zviradi");
}
