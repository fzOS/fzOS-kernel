#include <kernel.h>
#include <kerneldisplay.h>

void kernel_main(KernelInfo info) {
  graphics_init(info.gop);
  graphics_clear_screen(0x00000000);
  U32 default_font_color;
  default_font_color = 0xffffff00;
  kernel_log_print_string("Hello World! I am fzOS.\n", default_font_color);
  kernel_log_print_string("kernel version: fzKernel 0.1.0", default_font_color);
}