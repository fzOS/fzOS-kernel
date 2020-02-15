#include <uefivars.h>
#include <types.h>
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

U32 KERNEL_CONSOLE_FONT_POSITION_Y;
U32 KERNEL_CONSOLE_FONT_POSITION_X;
U32 KERNEL_CONSOLE_FONT_MAX_X;
U32 KERNEL_CONSOLE_FONT_MAX_Y;

void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);
void graphics_clear_screen(U32 color);
void graphics_fill_rect(int x, int y, int w, int h, U32 color);
void graphics_draw_pixel(int x, int y, U32 color);

#endif