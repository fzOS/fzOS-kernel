#include <drivers/graphics.h>
#include <types.h>
GraphicsData g_graphics_data;
//初始化gop接口
void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
  g_graphics_data.gop = gop;
  g_graphics_data.frame_buffer_base = (U32*)(gop->Mode->FrameBufferBase+KERNEL_ADDR_OFFSET);
  g_graphics_data.pixels_per_line = gop->Mode->Info->PixelsPerScanLine;
  g_graphics_data.pixels_vertical = gop->Mode->Info->VerticalResolution;
  g_graphics_data.default_background_color = 0x001e1e1e;
  //("graphics");
}

//清除屏幕
void graphics_clear_screen(U32 color) {
  graphics_fill_rect(0, 0, g_graphics_data.gop->Mode->Info->HorizontalResolution,
                     g_graphics_data.gop->Mode->Info->VerticalResolution, color);
  g_graphics_data.default_background_color = color;
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
  g_graphics_data.frame_buffer_base[y * g_graphics_data.pixels_per_line + x] =
      color;
}
