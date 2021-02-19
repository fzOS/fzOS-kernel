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
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
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
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
inline U32 inl(U16 port)
{
    U32 val;
    __asm__ volatile ("inl %1, %0":"=a"(val):"Nd"(port));
    return val;
}
