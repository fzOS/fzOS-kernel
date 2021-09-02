#include <coldpoint/automata/math_inst.h>
cpstatus opcode_add(thread* t)
{
    stack_var v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;

    switch(v1.type) {
        case STACK_TYPE_INT: {
            t->stack[t->rsp].data = v2.data+v1.data;
        }

    }
}
cpstatus opcode_sub(thread* t)
{

}
cpstatus opcode_mul(thread* t)
{

}
cpstatus opcode_div(thread* t)
{

}
cpstatus opcode_mod(thread* t)
{

}
cpstatus opcode_neg(thread* t)
{

}
