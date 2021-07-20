#ifndef GDT_H
#define GDT_H
#include <types.h>
#include <memory/memory.h>
//虽然其实没什么用
//但是AMD64为了保证与Intel的兼容还是一定要有这玩意……
//也是很无奈了TAT
//So F**k you, Intel!

void init_gdt(void);
#endif
