#include <efi.h>
#include <efilib.h>

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

uint32_t KERNEL_CONSOLE_FONT_POSITION_Y;
uint32_t KERNEL_CONSOLE_FONT_POSITION_X;
uint32_t KERNEL_CONSOLE_FONT_MAX_X;
uint32_t KERNEL_CONSOLE_FONT_MAX_Y;

void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);
void graphics_clear_screen(uint32_t color);
void graphics_fill_rect(int x, int y, int w, int h, uint32_t color);
void graphics_draw_pixel(int x, int y, uint32_t color);

#endif