#include <halt.h>
#include <printk.h>
inline void halt(void)
{
    __asm__("hlt");
}
inline void die(void)
{
    printk(" Exception caught! System halted.\n");
    __asm__("cli;" //die的时候要先关中断23333
                   //以及其他事务……
            "hlt;"
    );
}