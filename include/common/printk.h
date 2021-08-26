#ifndef PRINTK
#define PRINTK
#include <types.h>
#include <stdarg.h>
#include <drivers/fbcon.h>
//定义标准输入和输出终端。

extern console* default_console;

int printk(const char* format,...);

//定义一些常用的输出颜色。
#define CONSOLE_COLOR_RED    "%#f32525"
#define CONSOLE_COLOR_ORANGE "%#e36b11"
#define CONSOLE_COLOR_YELLOW "%#c8b923"
#define CONSOLE_COLOR_GREEN  "%#24a93e"
#define CONSOLE_COLOR_BLUE   "%#189bbf"
#define CONSOLE_COLOR_PURPLE "%#9b1ad5"
#define CONSOLE_COLOR_WHITE  "%#ffffff"
//如果我们在DEBUG模式下，启用debug函数。
#ifdef FZOS_DEBUG_SWITCH
#define debug(x...) printk(x)
#else
#define debug(x...)
#endif
#endif
