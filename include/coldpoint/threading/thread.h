#ifndef THREAD_H
#define THREAD_H
#include <types.h>
#include <coldpoint/common/class.h>
#define JVM_MAX_STACK_SIZE 1048576
typedef struct {
    code_attribute* code;
    U64 pc;
    U64 rsp;
    U64 rbp;
    U64 stack[JVM_MAX_STACK_SIZE/sizeof(U64)];
} thread;
/*
    我们的JVM栈结构：
    (RBP位置)
    |-return code*(U64)
    |-return pc(U64)
    |-return rsp(U64)
    |-return rbp(U64)
    |-local variables(U64*n)
    |-method stack(U64*m) <---(RSP位于0～m之间)
    (RBP+(4+n+m))
 */
typedef struct {
    code_attribute* return_code;
    U64 pc;
    U64 return_rsp;
    U64 return_rbp;
    U64 variables[0];
} stack_frame;
void thread_test(const class* c);
#endif
