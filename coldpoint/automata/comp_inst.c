#include <coldpoint/automata/comp_inst.h>
#include <common/math.h>
static inline void relative_jump(thread* t)
{
    U8 offset1=t->code->code[t->pc];
    U8 offset2=t->code->code[t->pc+1];
    t->pc-=1;
    short offset = (offset1<<8)|offset2;
    t->pc += offset;
}
cpstatus opcode_lcmp(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    print_opcode("lcmp %d<->%d\n",v1.data,v2.data);
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
cpstatus opcode_ifeq(thread* t)
{
    t->rsp -= 1;
    StackVar *v1=&t->stack[t->rsp];
    int val = (int)(v1->data);
    print_opcode("ifeq %d\n",val);
    if(val==0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifne(thread* t)
{
    StackVar v1=t->stack[t->rsp--];
    int val = (int)(v1.data);
    print_opcode("ifne %d\n",val);
    if(val!=0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iflt(thread* t)
{
    StackVar *v1=&t->stack[t->rsp--];
    int val = (int)(v1->data);
    print_opcode("iflt %d\n",val);
    if(val<0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifle(thread* t)
{
    StackVar *v1=&t->stack[t->rsp--];
    int val = (int)(v1->data);
    print_opcode("ifle %d\n",val);
    if(val<=0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifgt(thread* t)
{
    StackVar *v1=&t->stack[t->rsp--];
    int val = (int)(v1->data);
    print_opcode("ifgt %d\n",val);
    if(val>0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifge(thread* t)
{
    StackVar *v1=&t->stack[t->rsp--];
    int val = (int)(v1->data);
    print_opcode("ifge %d\n",val);
    if(val>=0) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmpeq(thread* t)
{
    print_opcode("ificmpeq\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    if(val1==val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmpne(thread* t)
{
    print_opcode("ificmpne\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    if(val1!=val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmplt(thread* t)
{
    print_opcode("ificmplt\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    if(val1<val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmple(thread* t)
{
    print_opcode("ificmple\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    if(val1<=val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmpgt(thread* t)
{
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    print_opcode("ificmpgt %d>%d?\n",val1,val2);
    if(val1>val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ificmpge(thread* t)
{
    print_opcode("ificmpge\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    int val1 = (int)(v1->data),val2 = (int)(v2->data);
    if(val1>=val2) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifacmpeq(thread* t)
{
    print_opcode("ifacmpeq\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    if(v1->data==v2->data) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifacmpne(thread* t)
{
    print_opcode("ifacmpne\n");
    StackVar *v2=&t->stack[t->rsp],*v1=&t->stack[t->rsp-1];
    t->rsp -= 2;
    if(v1->data!=v2->data) {
        relative_jump(t);
    }
    else {
        t->pc+=2;
    }
    return COLD_POINT_SUCCESS;
}
