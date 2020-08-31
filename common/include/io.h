#ifndef IO_H
#define IO_H
#include <types.h>
void outb(U16 port, U8 val);
void outw(U16 port, U16 val);
U8 inb(U16 port);
U16 inw(U16 port);
#endif