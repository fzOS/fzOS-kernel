#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/classloader.h>
cpstatus opcode_ldc2_w(thread* t)
{
    t->is_wide |= 0x1;
    return opcode_ldc(t);
}
cpstatus opcode_ldc(thread* t)
{
    U32 no;
    if(t->is_wide&0x1) {
        no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
        t->pc+=2;
        t->is_wide&=(~0x1);
    }
    else {
        no = t->code->code[t->pc];
        t->pc++;
    }
    print_opcode("ldc #%d:",no);
    ConstantEntry* const_entry = ((ConstantEntry*)(t->class->buffer+t->class->constant_entry_offset))+no;
    StackVar v2;
    double *p1 = (double*)(&(v2.data));
    switch(const_entry->type) {
        case CONSTANT_INT: {
            v2.data = ((IntegerInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_INT;
            print_opcode("type=int,val=%d\n",((IntegerInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_FLOAT: {
            *p1 = ((FloatInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_FLOAT;
            print_opcode("type=float,val=%f\n",((FloatInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_LONG: {
            v2.data = ((LongInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_LONG;
            print_opcode("type=long,val=%lld\n",((LongInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_DOUBLE: {
            *p1 = ((DoubleInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_DOUBLE;
            print_opcode("type=double,val=%lf\n",((DoubleInfoConstant*)const_entry)->val);
            break;
        }
        default: {
            print_opcode("Unknown type:%d.\n",const_entry->type);
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    t->stack[++(t->rsp)] = v2;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_new(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    print_opcode("new #%d:",no);
    ConstantEntry* const_entry = ((ConstantEntry*)(t->class->buffer+t->class->constant_entry_offset))+no;
    StackVar v2;
    v2.type = STACK_TYPE_REFERENCE;
    const U8* class_name = class_get_utf8_string(t->class,((ClassInfoConstant*)const_entry)->name_index);
    v2.data = (U64)new_object(getclass(class_name));
    t->stack[++(t->rsp)] = v2;
    print_opcode("%s:0x%x\n",class_name,v2.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_invokespecial(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    print_opcode("invokespecial #%d\n",no);


    return COLD_POINT_SUCCESS;
}
cpstatus opcode_getstatic(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    StackVar v1;
    (void)no;
    (void)v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_putstatic(thread* t)
{
    return COLD_POINT_SUCCESS;
}
