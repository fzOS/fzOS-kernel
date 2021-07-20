#ifndef IO_H
#define IO_H
#include <types.h>
void outb(U16 port, U8 val);
void outw(U16 port, U16 val);
void outl(U16 port, U32 val);
U8 inb(U16 port);
U16 inw(U16 port);
U32 inl(U16 port);
#endif
