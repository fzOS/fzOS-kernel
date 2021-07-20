
#ifndef _GRAPHICS_H
#define _GRAPHICS_H
#include <uefivars.h>
#include <types.h>
typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  U32 *frame_buffer_base;
  U32 pixels_per_line;
  U32 default_background_color;
} g_data; 

extern U32 KERNEL_CONSOLE_FONT_POSITION_Y;
extern U32 KERNEL_CONSOLE_FONT_POSITION_X;
extern U32 KERNEL_CONSOLE_FONT_MAX_X;
extern U32 KERNEL_CONSOLE_FONT_MAX_Y;
extern g_data graphics_data;
void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);
void graphics_clear_screen(U32 color);
void graphics_fill_rect(int x, int y, int w, int h, U32 color);
void graphics_draw_pixel(int x, int y, U32 color);
void kernel_display_move_up();

#endif