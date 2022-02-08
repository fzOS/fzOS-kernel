#include <coldpoint/automata/ctrl_inst.h>
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
    //skip the padding.
    if(t->pc&0x3) {
        t->pc = (t->pc&(~0x3)) +1;
    }

    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lookupswitch(thread* t);
