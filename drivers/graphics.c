#include <graphics.h>
#include <types.h>
U32 KERNEL_CONSOLE_FONT_POSITION_Y;
U32 KERNEL_CONSOLE_FONT_POSITION_X;
U32 KERNEL_CONSOLE_FONT_MAX_X;
U32 KERNEL_CONSOLE_FONT_MAX_Y;
g_data graphics_data;
//初始化gop接口
void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
  graphics_data.gop = gop;
  graphics_data.frame_buffer_base = (U32*)gop->Mode->FrameBufferBase;
  graphics_data.pixels_per_line = gop->Mode->Info->PixelsPerScanLine;
  graphics_data.default_background_color = 0x001e1e1e;
  //("graphics");
}

//清除屏幕
void graphics_clear_screen(U32 color) {
  graphics_fill_rect(0, 0, graphics_data.gop->Mode->Info->HorizontalResolution,
                     graphics_data.gop->Mode->Info->VerticalResolution, color);
  graphics_data.default_background_color = color;
}

//画方形，x起点，y起点，宽，高，颜色
void graphics_fill_rect(int x, int y, int w, int h, U32 color) {
  for (int j = y; j < y + h; ++j) {
    for (int i = x; i < x + w; ++i) {
      graphics_draw_pixel(i, j, color);
    }
  }
}

//向gop接口绘制像素
void graphics_draw_pixel(int x, int y, U32 color) {
  graphics_data.frame_buffer_base[y * graphics_data.pixels_per_line + x] =
      color;
}
