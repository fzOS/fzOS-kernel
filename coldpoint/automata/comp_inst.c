#include <coldpoint/automata/comp_inst.h>
#include <common/math.h>
cpstatus opcode_lcmp(thread* t)
{
    print_opcode("lcmp\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    if(v1.data>v2.data) {
        t->stack[t->rsp].data = 1;
    }
    else if(v1.data==v2.data) {
        t->stack[t->rsp].data = 0;
    }
    else {
        t->stack[t->rsp].data = -1;
    }
    t->stack[t->rsp].type = STACK_TYPE_INT;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_cmpg(thread* t)
{
    print_opcode("cmpg\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 1;
    double *d1 = (double*)(&(v1->data)),*d2 = (double*)(&(v2->data));
    if(*d1>*d2) {
        t->stack[t->rsp].data = 1;
    }
    else if(*d1==*d2) {
        t->stack[t->rsp].data = 0;
    }
    else if(*d1<*d2){
        t->stack[t->rsp].data = -1;
    }
    else {
        t->stack[t->rsp].data = 1;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_cmpl(thread* t)
{
    print_opcode("cmpl\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 1;
    double *d1 = (double*)(&(v1->data)),*d2 = (double*)(&(v2->data));
    if(*d1>*d2) {
        t->stack[t->rsp].data = 1;
    }
    else if(*d1==*d2) {
        t->stack[t->rsp].data = 0;
    }
    else if(*d1<*d2){
        t->stack[t->rsp].data = -1;
    }
    else {
        t->stack[t->rsp].data = -1;
    }
    return COLD_POINT_SUCCESS;
}
