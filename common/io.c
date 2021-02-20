#include <io.h>
inline void outb(U16 port, U8 val)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
inline U8 inb(U16 port)
{
    U8 val;
    __asm__ volatile ("inb %1, %0":"=a"(val):"Nd"(port));
    return val;
}
inline void outw(U16 port, U16 val)
{
    __asm__ volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}
inline U16 inw(U16 port)
{
    U16 val;
    __asm__ volatile ("inw %1, %0":"=a"(val):"Nd"(port));
    return val;
}
inline void outl(U16 port, U32 val)
{
    __asm__ volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}
inline U32 inl(U16 port)
{
    U32 val;
    __asm__ volatile ("inl %1, %0":"=a"(val):"Nd"(port));
    return val;
}
