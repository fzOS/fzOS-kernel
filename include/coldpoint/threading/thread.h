#ifndef THREAD_H
#define THREAD_H
#include <types.h>
#include <coldpoint/threading/process.h>
#include <coldpoint/common/class.h>
#define JVM_MAX_STACK_SIZE 1048576
typedef enum {
    STACK_TYPE_CODE_POINTER=0,
    STACK_TYPE_PC,
    STACK_TYPE_RSP,
    STACK_TYPE_RBP,
    STACK_TYPE_BYTE,
    STACK_TYPE_SHORT,
    STACK_TYPE_INT,
    STACK_TYPE_LONG,
    STACK_TYPE_FLOAT,
    STACK_TYPE_DOUBLE,
    STACK_TYPE_CHAR,
    STACK_TYPE_REFERENCE
} StackVarType;
typedef struct {
    StackVarType type;
    U64 data;
} StackVar;
typedef struct {
    process* p;
    U64 tid;
    U8 is_wide;
    CodeAttribute* code;
    U64 pc;
    U64 rsp;
    U64 rbp;
    StackVar stack[JVM_MAX_STACK_SIZE/sizeof(U64)];
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
    由于定长栈设计，将会发生类型提升。
    i.e. 整数类型-> U64;
         浮点类型-> double;
*/
typedef struct {
    StackVar return_code;//code_attribute*
    StackVar pc;
    StackVar return_rsp;
    StackVar return_rbp;
    StackVar variables[0];
} stack_frame;
void thread_test(const class* c);
#endif
