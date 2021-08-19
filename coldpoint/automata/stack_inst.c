#include <coldpoint/automata/stack_inst.h>
cpstatus opcode_pop(thread* t)
{
    print_opcode("pop\n");
    t->rsp--;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_pop2(thread* t)
{
    print_opcode("pop2\n");
    if(t->stack[t->rsp]&0xFFFFFFFF00000000) {
        goto pop2_sub_common;
    }
    t->rsp--;
pop2_sub_common:
    t->rsp--;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup(thread* t)
{
    print_opcode("dup\n");
    t->stack[t->rsp+1]=t->stack[t->rsp];
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup_x1(thread* t)
{
    print_opcode("dup_x1\n");
    t->stack[t->rsp+1]=t->stack[t->rsp];
    t->stack[t->rsp]=t->stack[t->rsp-1];
    t->stack[t->rsp-1]=t->stack[t->rsp+1];
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup_x2(thread* t)
{
    print_opcode("dup_x2\n");
    if(t->stack[t->rsp-1]&0xFFFFFFFF00000000) {
        t->stack[t->rsp+1]=t->stack[t->rsp];
        t->stack[t->rsp]=t->stack[t->rsp-1];
        t->stack[t->rsp-1]=t->stack[t->rsp+1];
        t->rsp++;
    }
    else {
        t->stack[t->rsp+1]=t->stack[t->rsp];
        t->stack[t->rsp]=t->stack[t->rsp-1];
        t->stack[t->rsp-1]=t->stack[t->rsp-2];
        t->stack[t->rsp-2]=t->stack[t->rsp+1];
        t->rsp++;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup2(thread* t)
{
    print_opcode("dup2\n");
    if(t->stack[t->rsp]&0xFFFFFFFF00000000) {
        t->stack[t->rsp+1]=t->stack[t->rsp];
        t->rsp++;
    }
    else {
        t->stack[t->rsp+1]=t->stack[t->rsp-1];
        t->stack[t->rsp+2]=t->stack[t->rsp];
        t->rsp+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup2_x1(thread* t)
{
    print_opcode("dup2_x1\n");
    if(t->stack[t->rsp-1]&0xFFFFFFFF00000000) {
        t->stack[t->rsp+1]=t->stack[t->rsp];
        t->stack[t->rsp]=t->stack[t->rsp-1];
        t->stack[t->rsp-1]=t->stack[t->rsp+1];
        t->rsp++;
    }
    else {
        t->stack[t->rsp+2]=t->stack[t->rsp];
        t->stack[t->rsp+1]=t->stack[t->rsp-1];
        t->stack[t->rsp]  =t->stack[t->rsp-2];
        t->stack[t->rsp-1]=t->stack[t->rsp+2];
        t->stack[t->rsp-2]=t->stack[t->rsp+1];
        t->rsp+=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dup2_x2(thread* t)
{
    if(t->stack[t->rsp-1]&0xFFFFFFFF00000000) {
        if(t->stack[t->rsp]&0xFFFFFFFF00000000) {
            t->stack[t->rsp+1]=t->stack[t->rsp];
            t->stack[t->rsp]=t->stack[t->rsp-1];
            t->stack[t->rsp-1]=t->stack[t->rsp+1];
            t->rsp++;
        }
        else {
            return COLD_POINT_NULL;
        }
    }
    else {
        if(t->stack[t->rsp]&0xFFFFFFFF00000000) {
            t->stack[t->rsp+1]=t->stack[t->rsp];
            t->stack[t->rsp]=t->stack[t->rsp-1];
            t->stack[t->rsp-1]=t->stack[t->rsp-2];
            t->stack[t->rsp-2]=t->stack[t->rsp+1];
            t->rsp++;
        }
        else {
            if(t->stack[t->rsp-2]&0xFFFFFFFF00000000) {
                t->stack[t->rsp+2] = t->stack[t->rsp];
                t->stack[t->rsp+1] = t->stack[t->rsp-1];
                t->stack[t->rsp] = t->stack[t->rsp-2];
                t->stack[t->rsp-1] = t->stack[t->rsp+2];
                t->stack[t->rsp-2] = t->stack[t->rsp+1];
            }
            else {
                t->stack[t->rsp+2] = t->stack[t->rsp];
                t->stack[t->rsp+1] = t->stack[t->rsp-1];
                t->stack[t->rsp] = t->stack[t->rsp-2];
                t->stack[t->rsp-1] = t->stack[t->rsp-3];
                t->stack[t->rsp-2] = t->stack[t->rsp+2];
                t->stack[t->rsp-3] = t->stack[t->rsp+1];
            }
            t->rsp+=2;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_swap(thread* t)
{
    U64 val = t->stack[t->rsp-1];
    t->stack[t->rsp-1]=t->stack[t->rsp];
    t->stack[t->rsp] = val;
    return COLD_POINT_SUCCESS;
}
