#include <coldpoint/automata/conv_inst.h>
cpstatus opcode_to_long(thread* t)
{
    print_opcode("i/l/f/d/2l\n");
    StackVar v1=t->stack[t->rsp];
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT: {
            double *var = ((double*)&v1.data);
            v1.data = (long) *var;
            break;
        }
        case STACK_TYPE_INT:
        case STACK_TYPE_LONG: {
            break;
        }
        default: {
            printk("Type %d cannot be converted into long!\n",v1.type);
            while(1);
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    v1.type = STACK_TYPE_LONG;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_to_int(thread* t)
{
    print_opcode("i/l/f/d/2i\n");
    StackVar v1=t->stack[t->rsp];
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT : {
            double *var = ((double*)&v1.data);
            v1.data  = (int) *var;
            break;
        }
        case STACK_TYPE_INT:
        case STACK_TYPE_LONG: {
            v1.data = (int)v1.data;
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
    print_opcode("i/l/f/d/2f\n");
    StackVar v1=t->stack[t->rsp];
    double *val = ((double*)&v1.data);
    switch(v1.type) {
        case STACK_TYPE_FLOAT:
        case STACK_TYPE_DOUBLE: {
            *val = (float)*val;
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            *val = (float)v1.data;
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
    print_opcode("i/l/f/d/2d\n");
    StackVar v1=t->stack[t->rsp];
    double *val = ((double*)&v1.data);
    switch(v1.type) {
        case STACK_TYPE_FLOAT:
        case STACK_TYPE_DOUBLE: {
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            *val = (double)v1.data;
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
cpstatus opcode_int_to_byte(thread* t)
{
    print_opcode("i2b\n");
    t->stack[t->rsp].data = (byte)t->stack[t->rsp].data;
    t->stack[t->rsp].type = STACK_TYPE_INT;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_int_to_char(thread* t)
{
    print_opcode("i2c\n");
    t->stack[t->rsp].data = (char)t->stack[t->rsp].data;
    t->stack[t->rsp].type = STACK_TYPE_CHAR;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_int_to_short(thread* t)
{
    print_opcode("i2s\n");
    t->stack[t->rsp].data = (short)t->stack[t->rsp].data;
    t->stack[t->rsp].type = STACK_TYPE_INT;
    return COLD_POINT_SUCCESS;
}
