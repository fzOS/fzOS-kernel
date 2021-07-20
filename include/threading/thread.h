#ifndef THREAD_H
#define THREAD_H
#include <types.h>
#include <threading/process.h>
#include <common/linkedlist.h>
typedef struct {
    U64 tid;
    union {
        U8 raw[16384-sizeof(U64)];//16k的栈
        struct {
            U64 rax;
            U64 rbx;
            U64 rcx;
            U64 rdx;
            U64 rsi;
            U64 rdi;
            U64 rbp;
            U64 r8;
            U64 r9;
            U64 r10;
            U64 r11;
            U64 r12;
            U64 r13;
            U64 r14;
            U64 r15;
        } registers;//寄存器
    } stack;
} thread;

#endif
