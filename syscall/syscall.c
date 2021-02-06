#include <syscall.h>
#include <printk.h>
void syscall_handler(U64 arg1)
{
    U64 what;
    __asm__ (
        "movq " "%%rax,%0\n"
        :"=g"(what)
        :
        :"%rax","memory"
    );
    printk("Syscalled for reason %d.\n",what);
    __asm__(
        "sysretq"
    );
}
#pragma GCC optimize 0
void init_syscall()
{
    /*
     
      mov rcx, 0xc0000080 ; EFER MSR
    rdmsr               ; read current EFER
    or eax, 1           ; enable SCE bit
    wrmsr               ; write back new EFER
    mov rcx, 0xc0000081 ; STAR MSR
    rdmsr               ; read current STAR
    mov edx, 0x00180008 ; load up GDT segment bases 0x0 (kernel) and 0x18 (user)
    wrmsr               ; write back new STAR
    ret          */
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
        "movq " "$0x00180008, %%rdx\n"
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
