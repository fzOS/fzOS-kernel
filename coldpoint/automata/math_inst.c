#include <coldpoint/automata/math_inst.h>
#include <stddef.h>
#define is_type_2(x) ((x.type==STACK_TYPE_LONG)||(x.type==STACK_TYPE_DOUBLE))
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wswitch"
cpstatus opcode_add(thread* t)
{
    print_opcode("i/l/f/d/add\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 += *p2;
            if(v1.type==STACK_TYPE_FLOAT) {
                *p1 = (float)*p1;
            }
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            long val1=(long)v1.data;
            long val2=(long)v2.data;
            v1.data = val1+val2;
            print_opcode("%d+%d=%d\n",val1,val2,v1.data);
            if(v1.type==STACK_TYPE_INT) {
                v1.data = (int)v1.data;
            }
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_sub(thread* t)
{
    print_opcode("i/l/f/d/sub\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 -= *p2;
            if(v1.type==STACK_TYPE_FLOAT) {
                *p1 = (float)*p1;
            }
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            long val1=(long)v1.data;
            long val2=(long)v2.data;
            v1.data = val1-val2;
            print_opcode("%d-%d=%d\n",val1,val2,v1.data);
            if(v1.type==STACK_TYPE_INT) {
                v1.data = (int)v1.data;
            }
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_mul(thread* t)
{
    print_opcode("i/l/f/d/mul\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            *p1 *= *p2;
            if(v1.type==STACK_TYPE_FLOAT) {
                *p1 = (float)*p1;
            }
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            long val1=(long)v1.data;
            long val2=(long)v2.data;
            v1.data = val1*val2;
            print_opcode("%d*%d=%d\n",val1,val2,v1.data);
            if(v1.type==STACK_TYPE_INT) {
                v1.data = (int)v1.data;
            }
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_div(thread* t)
{
    print_opcode("i/l/f/d/div\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_DOUBLE:
        case STACK_TYPE_FLOAT: {
            double *p1 = (double*)(&(v1.data)),*p2 = (double*)(&(v2.data));
            if(*p2==0.0) {
                except(t,"Div by 0");
                return COLD_POINT_EXEC_FAILURE;
            }
            *p1 /= *p2;
            if(v1.type==STACK_TYPE_FLOAT) {
                *p1 = (float)*p1;
            }
            break;
        }
        case STACK_TYPE_LONG:
        case STACK_TYPE_INT: {
            long val1=(long)v1.data;
            long val2=(long)v2.data;
            if(val2==0) {
                except(t,"Div by 0");
                return COLD_POINT_EXEC_FAILURE;
            }
            v1.data = val1/val2;
            print_opcode("%d/%d=%d\n",val1,val2,v1.data);
            if(v1.type==STACK_TYPE_INT) {
                v1.data = (int)v1.data;
            }
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_mod(thread* t)
{
    print_opcode("i/l/f/d/mod\n");
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
        case STACK_TYPE_INT:
        case STACK_TYPE_LONG: {
            print_opcode("%d mod %d=%d\n",v1.data,v2.data,v1.data%v2.data);
            v1.data %= v2.data;
            if(v1.type ==STACK_TYPE_INT) {
                v1.data = (int)v1.data;
            }
            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_neg(thread* t)
{
    print_opcode("i/l/f/d/neg\n");
    StackVar v1=t->stack[t->rsp];
    switch(v1.type) {
        case STACK_TYPE_FLOAT:
        case STACK_TYPE_DOUBLE: {
            double *p1 = (double*)(&(v1.data));
            *p1 = -*p1;
            break;
        }
        case STACK_TYPE_INT:
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
    print_opcode("i/l/f/d/shl\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    print_opcode("%d,%d\n",v1.type,v2.type);
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT:
        case STACK_TYPE_LONG: {
            print_opcode("%d<<%d=%d\n",v1.data,v2.data,v1.data<<v2.data);
            v1.data <<= v2.data;

            break;
        }
    }
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_shr(thread* t)
{
    print_opcode("i/l/f/d/shr\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            int p1 = v1.data,p2 = v2.data;
            p1 >>= p2;
            v1.data = p1;
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
    print_opcode("i/l/f/d/ushr\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    switch(v1.type) {
        case STACK_TYPE_INT: {
            unsigned int p1 = v1.data;
            unsigned int p2 = v2.data;
            v1.data =(p1 >> p2);
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
    print_opcode("i/l/and\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    print_opcode("%d&%d=%d\n",v1.data,v2.data,v1.data&v2.data);
    v1.data &= v2.data;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_or(thread* t)
{
    print_opcode("i/l/or\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    t->rsp -= 1;
    print_opcode("%d|%d=%d\n",v1.data,v2.data,v1.data|v2.data);
    v1.data |= v2.data;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_xor(thread* t)
{
    print_opcode("i/l/xor\n");
    StackVar v2=t->stack[t->rsp],v1=t->stack[t->rsp-1];
    print_opcode("%d^%d=%d\n",v1.data,v2.data,v1.data<<v2.data);
    t->rsp -= 1;
    v1.data ^= v2.data;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_inc(thread* t)
{
    U32 index;
    I32 val;
    if(t->is_wide&0x01) {
        index = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
        t->pc+=2;
        val = (I16)((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
        t->pc+=2;
        t->is_wide&=(~0x1);
    }
    else {
        index = t->code->code[t->pc];
        val = (I8)t->code->code[t->pc+1];
        t->pc+=2;
    }
    print_opcode("i/l/inc %d,delta=%d\n",index,val);
    U64 stack_offset = t->rbp+offsetof(stack_frame,variables)/sizeof(StackVar)+index;
    t->stack[stack_offset].data += val;
    if(t->stack[stack_offset].type==STACK_TYPE_INT) {
        t->stack[stack_offset].data = (int)t->stack[stack_offset].data;
    }
    return COLD_POINT_SUCCESS;
}
