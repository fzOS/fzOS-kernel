#include <syscall/syscall.h>
#include <common/printk.h>
void userspace(){
    for(;;);
}
void __attribute__ ((noreturn)) syscall_handler (U64 arg1)
{
    register U64 what;
    __asm__ (
        "pushq " "%%rcx\n"
        "pushq " "%%r11\n"
        "movq " "%%rax,%0\n"
        :"=r"(what)
        :
        :"%rax","memory"
    );
    
    printk("Syscalled for reason %d.\n",what);
    __asm__(
        "popq " "%%r11\n"
        "popq " "%%rcx\n"
        "addq " "$0x08,%%rsp\n" //栈平衡
        "sysretq\n"
        :
        :
        :
    );
    //解决gcc的noreturn问题
    while(1){};
}
void  __attribute__((optimize("O0"))) init_syscall() 
{
    void (*syscall_handler_pointer) = syscall_handler;
    register  U64  syscall_handler_address = (U64) syscall_handler_pointer;
    syscall_handler_address |= 0xFFFF800000000000;
    __asm__ (
        "movq " "$0xc0000080, %%rcx\n"
        "rdmsr\n"
        "or " "$0x01,%%eax\n"
        "wrmsr\n"
        "movq " "$0xc0000081, %%rcx\n"
        "rdmsr\n"
        "movq " "$0x001b0008, %%rdx\n"
        "wrmsr\n"
        "movq " "$0xc0000082, %%rcx\n"
        "rdmsr\n"
        "movq " "%0,%%rax\n"
        "movq " "%1,%%rdx\n"
        "wrmsr\n"
        :
        :"g"(syscall_handler_address&0x00000000FFFFFFFF),"g"((syscall_handler_address&0xFFFFFFFF00000000)>>32)
        :"%rax","%rcx","%rdx","memory"
    );
}
