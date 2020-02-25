#include <kernel.h>
#include <kerneldisplay.h>

void kernel_main(KernelInfo info) {
  graphics_init(info.gop);
  graphics_clear_screen(0x00eed0db);
  U32 default_font_color;
  default_font_color = 0x00ffffff;
  int flagX;
  flagX = 0;
  for (int j = 0; j < KERNEL_CONSOLE_FONT_MAX_Y; j++) {
    for (int i = 0; i < KERNEL_CONSOLE_FONT_MAX_X; i++) {
      if (flagX == 0){
        kernel_draw_char_pos(default_font_color);
        flagX = 1;
      }else{
        flagX = 0;
      }
    }
  }
  kernel_log_print_char("A",default_font_color);
  kernel_log_print_string("Hello World! I am fzOS.\n", default_font_color);
  kernel_log_print_string("kernel version: fzKernel 0.1.0", default_font_color);
}