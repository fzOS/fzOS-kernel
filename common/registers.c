#include <registers.h>
U64 rdmsr(U64 opcode) 
{
    U64 low=0,high=0;
    __asm__  (
        "movq %0,%%rcx\n"
        "rdmsr\n"
        "movq %%rdx,%0\n"
        "movq %%rax,%1\n"
        :"=g"(high), "=g"(low)
        :"g" (opcode)
        :"%rcx","%rdx","%rax","memory"
    );
    return ((high&0xFFFFFFFF)<<32)|(low&0xFFFFFFFF);
}
void wrmsr(U64 opcode,U64 value)
{
    __asm__ (
        "movq %0,%%rcx\n"
        "movq %1,%%rdx\n"
        "movq %2,%%rax\n"
        "wrmsr\n"
        :
        :"g"(opcode),"g"((value&0xFFFFFFFF00000000ULL)),"g"((value&0x00000000FFFFFFFFULL))
        :"%rcx","%rdx","%rax","memory"
    );
}