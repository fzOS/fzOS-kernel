#ifndef WINDOW_CONSOLE
#define WINDOW_CONSOLE
#include <drivers/chardev.h>
void hook_window_console(void);
void restore_window_console(void);
void window_console_putchar(CharDev* dev, U8 c);
void window_console_flush(CharDev* dev);
#endif
