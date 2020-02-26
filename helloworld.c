#include <kernel.h>
#include <kerneldisplay.h>

void kernel_main(KernelInfo info) {
  graphics_init(info.gop);
  graphics_clear_screen(0x001e1e1e);
  U32 default_font_color;
  default_font_color = 0x00ffffff;
  kernel_log_print_string(" Hello World! I am fzOS.", default_font_color);
  kernel_log_print_string("\n Kernel version: fzKernel-0.1.2", default_font_color);
  kernel_log_print_line_cut('+', default_font_color);
  kernel_log_print_string("\n Kernel lowest address: ", default_font_color);
  kernel_print_U64_hex(info.kernel_lowest_address, default_font_color);
  kernel_log_print_string("\n Kernel Memory Usage: ", default_font_color);
  kernel_print_U64_hex(info.kernel_page_count, default_font_color);
  kernel_log_print_string("\n Memory map pointer: ", default_font_color);
  U64 memorymappointer;
  memorymappointer = (U64) info.memory_map;
  kernel_print_U64_hex(memorymappointer, default_font_color);
  kernel_log_print_string("\n Memory map size: ", default_font_color);
  kernel_print_U64_hex(info.mem_map_size, default_font_color);
  kernel_log_print_string("\n Memory Map Descriptor: ", default_font_color);
  kernel_print_U64_hex(info.mem_map_descriptor_size, default_font_color);
  U64 *memmappointer;
  memmappointer = (U64 *) info.memory_map;
  kernel_log_print_string("\n Memory Map: ", default_font_color);
  for (U8 i = 0; i < (info.mem_map_size / info.mem_map_descriptor_size); i++){
    kernel_log_print_string("\n #Map", default_font_color);
    kernel_log_print_num((U64) i, default_font_color);
    kernel_log_print_char(':', default_font_color);
    kernel_print_U64_hex(*memmappointer, default_font_color);
    memmappointer = memmappointer + 1;
    kernel_print_U64_hex(*memmappointer, default_font_color);
    memmappointer = memmappointer + 1;
    kernel_print_U64_hex(*memmappointer, default_font_color);
    memmappointer = memmappointer + 1;
    kernel_print_U64_hex(*memmappointer, default_font_color);
    memmappointer = memmappointer + 1;
    kernel_print_U64_hex(*memmappointer, default_font_color);
    memmappointer = memmappointer + 2;
  }
}