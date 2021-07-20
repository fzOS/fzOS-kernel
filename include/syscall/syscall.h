//系统调用变量定义。
#ifndef SYSCALL
#define SYSCALL

#include <types.h>

void init_syscall();

//定义系统调用编号。
#define SYSCALL_OPEN 1
#define SYSCALL_READ 2
#define SYSCALL_WRITE 3
#define SYSCALL_CLOSE 4
#endif
