#include <coldpoint/automata/math_inst.h>
#define is_type_2(x) ((x.type==STACK_TYPE_LONG)||(x.type==STACK_TYPE_DOUBLE))
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wswitch"
cpstatus opcode_add(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 += *p2;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float *p1 = (float*)(&(v1.data)),*p2 = (float*)(&(v2.data));
            *p1 += *p2;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 += *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data += v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_sub(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 -= *p2;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float *p1 = (float*)(&(v1.data)),*p2 = (float*)(&(v2.data));
            *p1 -= *p2;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 -= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data -= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_mul(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 *= *p2;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float *p1 = (float*)(&(v1.data)),*p2 = (float*)(&(v2.data));
            *p1 *= *p2;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 *= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data *= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_div(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 /= *p2;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float *p1 = (float*)(&(v1.data)),*p2 = (float*)(&(v2.data));
            *p1 /= *p2;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 /= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data /= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_mod(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case STACK_TYPE_FLOAT : {
            return COLD_POINT_NOT_IMPLEMENTED;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 %= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data %= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_neg(thread* t)
{
    StackVar v1=t->stack[t->rsp];
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data));
            *p1 = -*p1;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float *p1 = (float*)(&(v1.data));
            *p1 = -*p1;
            break;
        }
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data));
            *p1 = -*p1;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data = -v1.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_shl(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 <<= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data <<= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_shr(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            int *p1 = (int*)(&(v1.data)),*p2 = (int*)(&(v2.data));
            *p1 >>= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data >>= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ushr(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            unsigned int *p1 = (unsigned int*)(&(v1.data)),*p2 = (unsigned int*)(&(v2.data));
            *p1 >>= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data >>= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_and(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            unsigned int *p1 = (unsigned int*)(&(v1.data)),*p2 = (unsigned int*)(&(v2.data));
            *p1 &= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data &= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_or(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            unsigned int *p1 = (unsigned int*)(&(v1.data)),*p2 = (unsigned int*)(&(v2.data));
            *p1 |= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data |= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_xor(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            unsigned int *p1 = (unsigned int*)(&(v1.data)),*p2 = (unsigned int*)(&(v2.data));
            *p1 ^= *p2;
            break;
        }
        case STACK_TYPE_LONG: {
            v1.data ^= v2.data;
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_inc(thread* t)
{
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    StackVar v3=t->stack[t->rbp+v1.data];
    v3.data += v2.data;
    t->stack[t->rbp+v1.data] = v3;
}
