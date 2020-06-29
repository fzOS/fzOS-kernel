#ifndef PRINTK
#define PRINTK
#include <types.h>
#include <stdarg.h>
#include <chardev.h>
extern char_dev stdio;
//定义标准输入和输出终端。



int printk(char* format,...);
#endif