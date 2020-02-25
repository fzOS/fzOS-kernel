#include <kernel.h>
#include <kerneldisplay.h>

void kernel_main(KernelInfo info) {
  graphics_init(info.gop);
  graphics_clear_screen(0x001e1e1e);
  U32 default_font_color;
  default_font_color = 0x00ffffff;
  kernel_log_print_string(" Hello World! I am fzOS.", default_font_color);
  kernel_log_print_string("\n Kernel version: fzKernel-0.1.0", default_font_color);
  kernel_log_print_string("\n Memory map size: ", default_font_color);
  kernel_print_U64_hex(info.mem_map_size, default_font_color);
  kernel_log_print_string("\n Kernel lowest address: ", default_font_color);
  kernel_print_U64_hex(info.kernel_lowest_address, default_font_color);
}