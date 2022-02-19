#include <coldpoint/automata/load_inst.h>
#include <stddef.h>
//FIXME:重写！咕咕 #3
inline cpstatus load_internal(thread* t,int no)
{
    StackVar* const_val_entry = &t->stack[t->rbp+offsetof(stack_frame,variables)];
    t->rsp++;
    t->stack[t->rsp].data = const_val_entry[no].data;
    t->stack[t->rsp].type = const_val_entry[no].type;
    print_opcode("no.%d->%d(%d)\n",no,const_val_entry[no].data,t->rsp,t->rsp);
    return COLD_POINT_SUCCESS;
}
inline cpstatus store_internal(thread* t,int no)
{
    StackVar* const_val_entry = &t->stack[t->rbp+offsetof(stack_frame,variables)];
    const_val_entry[no].data = t->stack[t->rsp].data;
    const_val_entry[no].type = t->stack[t->rsp].type;
    print_opcode("no.%d<-%d(%d)\n",no,const_val_entry[no].data,t->rsp);
    t->rsp--;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_load(thread* t)
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
    print_opcode("i/l/f/d/aload %d\n",no);
    return load_internal(t,no);
}
cpstatus opcode_load0(thread* t)
{
    print_opcode("i/l/f/d/aload 0\n");
    return load_internal(t,0);
}
cpstatus opcode_load1(thread* t)
{
    print_opcode("i/l/f/d/aload 1\n");
    return load_internal(t,1);
}
cpstatus opcode_load2(thread* t)
{
    print_opcode("i/l/f/d/aload 2\n");
    return load_internal(t,2);
}
cpstatus opcode_load3(thread* t)
{
    print_opcode("i/l/f/d/aload 3\n");
    return load_internal(t,3);
}
cpstatus opcode_store(thread* t)
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
    print_opcode("i/l/f/d/astore %d\n",no);
    return store_internal(t,no);
}
cpstatus opcode_store0(thread* t)
{
    print_opcode("i/l/f/d/astore 0\n");
    return store_internal(t,0);
}
cpstatus opcode_store1(thread* t)
{
    print_opcode("i/l/f/d/astore 1\n");
    return store_internal(t,1);
}
cpstatus opcode_store2(thread* t)
{
    print_opcode("i/l/f/d/astore 2\n");
    return store_internal(t,2);
}
cpstatus opcode_store3(thread* t)
{
    print_opcode("i/l/f/d/astore 3\n");
    return store_internal(t,3);
}
