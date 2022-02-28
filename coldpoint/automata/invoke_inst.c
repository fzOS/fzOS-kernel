#include <coldpoint/automata/invoke_inst.h>
#include <coldpoint/threading/thread.h>
#include <coldpoint/classloader.h>
#include <coldpoint/automata/automata.h>
inline void invoke_method(thread* t,class* target_class,CodeAttribute* code_attr,U64 param_count)
{
    U64 new_stack_size_required = sizeof(stack_frame)/sizeof(StackVar)+param_count;
    U64 new_rbp = t->rsp-param_count;
    U64 new_rsp = new_rbp+new_stack_size_required;

    for(U64 i=param_count-1;i>=0;i--) {
        t->stack[new_rsp+i] = t->stack[new_rbp+i];
    }
    new_rsp += param_count;
    /*
    我们的JVM栈结构：
    (RBP位置)
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
    t->stack[new_rbp].data = (U64)target_class;
    t->stack[new_rbp].type = STACK_TYPE_CLASS_REF;
    t->stack[new_rbp+1].data = (U64)code_attr;
    t->stack[new_rbp+1].type = STACK_TYPE_CODE_POINTER;
    t->stack[new_rbp+2].data = t->pc;
    t->stack[new_rbp+2].type = STACK_TYPE_PC;
    t->stack[new_rbp+3].data = t->rsp;
    t->stack[new_rbp+3].type = STACK_TYPE_RSP;
    t->stack[new_rbp+4].data = t->rbp;
    t->stack[new_rbp+4].type = STACK_TYPE_RBP;
    t->class = target_class;
    t->code = code_attr;
    t->pc = 0;
    t->rsp = new_rsp;
    t->rbp = new_rbp;
    return;
}
cpstatus opcode_invokespecial(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    print_opcode("invokespecial #%d\n",no);


    return COLD_POINT_SUCCESS;
}
