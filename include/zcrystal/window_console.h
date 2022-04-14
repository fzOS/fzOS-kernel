#ifndef WINDOW_CONSOLE
#define WINDOW_CONSOLE
#include <drivers/chardev.h>
#include <zcrystal/window.h>
void hook_window_console(void);
void restore_window_console(void);
void set_window_console_window(Window* w,int fore_color,int back_color);
void window_console_putchar(CharDev* dev, U8 c);
void window_console_flush(CharDev* dev);
#endif
