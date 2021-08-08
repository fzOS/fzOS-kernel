#ifndef PRINTK
#define PRINTK
#include <types.h>
#include <stdarg.h>
#include <drivers/fbcon.h>
//定义标准输入和输出终端。

extern console* default_console;

int printk(const char* format,...);

//如果我们在DEBUG模式下，启用debug函数。
#ifdef FZOS_DEBUG_SWITCH
#define debug(x...) printk(x)
#else
#define debug(x...)
#endif
#endif
