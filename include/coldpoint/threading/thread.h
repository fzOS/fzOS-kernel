#ifndef THREAD_H
#define THREAD_H
#include <types.h>
#include <coldpoint/threading/process.h>
#include <coldpoint/common/class.h>
#include <drivers/console.h>
#include <common/semaphore.h>
#include <common/linkedlist.h>
#define JVM_MAX_STACK_SIZE 16384
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
    STACK_TYPE_REFERENCE,
    STACK_TYPE_CLASS_REF,
    STACK_TYPE_THREAD_STATUS
} StackVarType;
typedef struct {
    StackVarType type;
    U64 data;
} StackVar;
typedef enum {
    THREAD_RUNNING=0,
    THREAD_READY  =1,
    THREAD_BLOCKED=2,
    THREAD_TERMINATED=4
} ThreadStatus;
typedef enum {
    EXIT_SUCCESS,
    EXIT_KILLED,
    EXIT_EXCEPT
} ThreadExitStatus;
typedef struct {
    process* process;
    U64 tid;
    U8 is_wide;
    U8 status;
    semaphore* sem;
    Console* console;
    class* class;
    CodeAttribute* code;
    U64 pc;
    U64 rsp;
    U64 rbp;
    StackVar stack[JVM_MAX_STACK_SIZE/sizeof(U64)];
} thread;
/*
    我们的JVM栈结构：
    (RBP位置)
    |-return thread status(U64)
    |-return class*(U64)
    |-return code*(U64)
    |-return pc(U64)
    |-return rsp(U64)
    |-return rbp(U64)
    |-local variables(U64*n)
    |-method stack(U64*m) <---(RSP位于0～m之间)
    (RBP+(5+n+m))
    由于定长栈设计，将会发生类型提升。
    i.e. 整数类型-> U64;
         浮点类型-> double;
*/
typedef struct {
    StackVar return_thread_status;
    StackVar return_class;
    StackVar return_code;//code_attribute*
    StackVar pc;
    StackVar return_rsp;
    StackVar return_rbp;
    StackVar variables[0];
} stack_frame;
typedef struct {
    InlineLinkedListNode node;
    thread t;
} ThreadInlineLinkedListNode;
extern ThreadInlineLinkedListNode* g_current_thread;
extern InlineLinkedList g_thread_list;
process* create_process(void);
thread* create_thread(process* p,CodeAttribute* c,class* class,Console* con);
void destroy_process(process* p);
void thread_test(class* c);
void destroy_thread(thread* t);
thread* get_next_thread(void);
thread* get_thread_by_tid(U64 tid);
FzOSResult terminate_thread(thread* t,ThreadExitStatus status);
#endif
