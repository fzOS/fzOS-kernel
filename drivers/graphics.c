#include <drivers/graphics.h>
#include <drivers/vmsvga.h>
#include <types.h>
GraphicsData g_graphics_data;
int g_move_up_accelerated;
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
    int actual_x,actual_y;
    actual_x = (x+w>g_graphics_data.pixels_per_line)?g_graphics_data.pixels_per_line-w:x;
    actual_y = (y+h>g_graphics_data.pixels_vertical)?g_graphics_data.pixels_vertical-h:y;
    for (int j = actual_y; j < actual_y + h; ++j) {
      for (int i = actual_x; i < actual_x + w; ++i) {
        graphics_draw_pixel(i, j, color);
      }
    }
}

//向gop接口绘制像素
inline void graphics_draw_pixel(int x, int y, U32 color)
{
    g_graphics_data.frame_buffer_base[y * g_graphics_data.pixels_per_line + x] = color;
}
void graphics_move_up(int delta)
{
    if(g_move_up_accelerated) {
        return vmsvga_console_move_up(delta);
    }
    else {
        int newLine = g_graphics_data.gop->Mode->Info->VerticalResolution - delta;
        for (int i = 0; i < g_graphics_data.gop->Mode->Info->VerticalResolution; i++){
            for (int j = 0; j < g_graphics_data.gop->Mode->Info->HorizontalResolution; j++){
                if (i >= newLine){
                    g_graphics_data.frame_buffer_base[i * g_graphics_data.pixels_per_line + j] = g_graphics_data.default_background_color;
                }else{
                    g_graphics_data.frame_buffer_base[i * g_graphics_data.pixels_per_line + j] = g_graphics_data.frame_buffer_base[(i + delta) * g_graphics_data.pixels_per_line + j];
                }
            }
        }
    }
    g_screen_dirty = 1;
}
