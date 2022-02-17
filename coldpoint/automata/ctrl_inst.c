#include <coldpoint/automata/ctrl_inst.h>
#include <common/bswap.h>
static inline void relative_jump(thread* t)
{
    U8 offset1=t->code->code[t->pc];
    U8 offset2=t->code->code[t->pc+1];
    t->pc+=2;
    short offset = (offset1<<8)|offset2;
    t->pc += offset;
}
cpstatus opcode_goto(thread* t)
{
    relative_jump(t);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_tableswitch(thread* t)
{
    StackVar v1=t->stack[t->rsp];
    t->rsp--;
    U64 orig_pc=t->pc;
    //skip the padding.
    if(t->pc&0x3) {
        t->pc = (t->pc&(~0x3)) +1;
    }
    //Default Offset.
    I32* p = (I32*)&t->code->code[t->pc];
    I32 default_offset = bswap32(*p);
    p++;
    I32 high_val = bswap32(*p);
    p++;
    I32 low_val = bswap32(*p);
    p++;
    I32 desired_offset;
    if(((I32)v1.data)>=high_val||((I32)v1.data)<=low_val) {
        desired_offset = default_offset;
    }
    else {
        desired_offset = bswap32(p[((I32)v1.data)-low_val]);
    }
    t->pc = orig_pc + desired_offset;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lookupswitch(thread* t);
