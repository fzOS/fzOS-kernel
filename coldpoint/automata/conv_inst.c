#include <coldpoint/automata/conv_inst.h>
cpstatus opcode_to_long(thread* t)
{
    stack_var v1=t->stack[t->rsp];
    long* val = (long*)&(v1.data);
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double var = *((double*)&v1.data);
            *val = (long) var;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float var = *((float*)&v1.data);
            *val = (long) var;
            break;
        }
        case STACK_TYPE_INT: {
            int var = *((int*)&v1.data);
            *val = (long) var;
            break;
        }
        case STACK_TYPE_LONG: {
            break;
        }
        default: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    v1.type = STACK_TYPE_LONG;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_to_int(thread* t)
{
    stack_var v1=t->stack[t->rsp];
    int* val = (int*)&(v1.data);
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double var = *((double*)&v1.data);
            *val = (int) var;
            break;
        }
        case STACK_TYPE_FLOAT : {
            float var = *((float*)&v1.data);
            *val = (int) var;
            break;
        }
        case STACK_TYPE_INT: {
            break;
        }
        case STACK_TYPE_LONG: {
            long var = *((long*)&v1.data);
            *val = (int) var;
            break;
        }
        default: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    v1.type = STACK_TYPE_INT;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_to_float(thread* t)
{
    stack_var v1=t->stack[t->rsp];
    float* val = (float*)&(v1.data);
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            double var = *((double*)&v1.data);
            *val = (float) var;
            break;
        }
        case STACK_TYPE_FLOAT : {
            break;
        }
        case STACK_TYPE_INT: {
            int var = *((int*)&v1.data);
            *val = (float) var;
            break;
        }
        case STACK_TYPE_LONG: {
            long var = *((long*)&v1.data);
            *val = (float) var;
            break;
        }
        default: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    v1.type = STACK_TYPE_FLOAT;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_to_double(thread* t)
{
    stack_var v1=t->stack[t->rsp];
    double* val = (double*)&(v1.data);
    switch(v1.type) {
        case STACK_TYPE_DOUBLE: {
            break;
        }
        case STACK_TYPE_FLOAT : {
            float var = *((float*)&v1.data);
            *val = (double) var;
            break;
        }
        case STACK_TYPE_INT: {
            int var = *((int*)&v1.data);
            *val = (double) var;
            break;
        }
        case STACK_TYPE_LONG: {
            long var = *((long*)&v1.data);
            *val = (double) var;
            break;
        }
        default: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    v1.type = STACK_TYPE_DOUBLE;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
