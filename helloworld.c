#include <kernel.h>
#include <kerneldisplay.c>

void kernel_main(KernelInfo info) {
  graphics_init(info.gop);
  graphics_clear_screen(0x00000000);
  uint32_t default_font_color;
  default_font_color = 0xffffff00;
  kernel_log_print_char('H', default_font_color);
  kernel_log_print_char('e', default_font_color);
  kernel_log_print_char('l', default_font_color);
  kernel_log_print_char('l', default_font_color);
  kernel_log_print_char('o', default_font_color);
  kernel_log_print_char(' ', default_font_color);
  kernel_log_print_char('W', default_font_color);
  kernel_log_print_char('o', default_font_color);
  kernel_log_print_char('r', default_font_color);
  kernel_log_print_char('l', default_font_color);
  kernel_log_print_char('d', default_font_color);
  kernel_log_print_char('!', default_font_color);
}