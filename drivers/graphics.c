#include <graphics.h>
#include <types.h>
static struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  U32 *frame_buffer_base;
  U32 pixels_per_line;
} graphics_data;

//初始化gop接口
void graphics_init(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
  graphics_data.gop = gop;
  graphics_data.frame_buffer_base = (U32*)gop->Mode->FrameBufferBase;
  graphics_data.pixels_per_line = gop->Mode->Info->PixelsPerScanLine;
  KERNEL_CONSOLE_FONT_MAX_X = graphics_data.pixels_per_line / 8 - 1;
  KERNEL_CONSOLE_FONT_MAX_Y = graphics_data.gop->Mode->Info->VerticalResolution / 16 - 1;
  KERNEL_CONSOLE_FONT_POSITION_Y = 0;
  KERNEL_CONSOLE_FONT_POSITION_X = 0;
  //("graphics");
}

//清除屏幕
void graphics_clear_screen(U32 color) {
  graphics_fill_rect(0, 0, graphics_data.gop->Mode->Info->HorizontalResolution,
                     graphics_data.gop->Mode->Info->VerticalResolution, color);
}

//画方形，y起点，x起点，宽，高，颜色
void graphics_fill_rect(int x, int y, int w, int h, U32 color) {
  for (int i = y; i < y + h; ++i) {
    for (int j = x; j < x + w; ++j) {
      graphics_draw_pixel(i, j, color);
    }
  }
}

//向gop接口绘制像素
void graphics_draw_pixel(int x, int y, U32 color) {
  graphics_data.frame_buffer_base[y * graphics_data.pixels_per_line + x] =
      color;
}
