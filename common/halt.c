#include <halt.h>
#include <printk.h>
inline void halt(void)
{
    __asm__("hlt");
}
inline void die(char* reason,interrupt_frame* frame)
{
    printk(" Exception caught:%s.\n",reason);
    printk(" RIP is at %x.\n",frame->RIP);
    printk(" System halted.\n");
    __asm__("cli;" //die的时候要先关中断23333
                   //以及其他事务……
            "hlt;"
    );
}