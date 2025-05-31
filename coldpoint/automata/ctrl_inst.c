#include <coldpoint/automata/ctrl_inst.h>
#include <common/bswap.h>
cpstatus opcode_goto(thread* t)
{
    U8 offset1=t->code->code[t->pc];
    U8 offset2=t->code->code[t->pc+1];
    short offset = (offset1<<8)|offset2;
    t->pc += offset-1;
    print_opcode("goto\n");
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
    print_opcode("tableswitch %d [%d,%d]\n",(I32)v1.data,low_val,high_val);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lookupswitch(thread* t)
{
    StackVar v1=t->stack[t->rsp];
    t->rsp--;
    U64 orig_pc=t->pc-1;
    //skip the padding.
    if(t->pc&0x3) {
        t->pc = (t->pc&(~0x3))+4;
    }
    //Default Offset.
    I32* p = (I32*)&t->code->code[t->pc];
    I32 default_offset = bswap32(*p);
    p++;
    U32 pairs = bswap32(*p);
    p++;
    struct pair {
        I32 match;
        I32 offset;
    } __attribute__((packed)) *pair = (struct pair*)p;
    for(U32 i=0;i<pairs;i++) {
        if(bswap32(pair->match)==(I32)v1.data) {
            t->pc = orig_pc+bswap32(pair->offset);
            return COLD_POINT_SUCCESS;
        }
        pair++;
    }
    t->pc = orig_pc+default_offset;
    print_opcode("lookupswitch %d\n",(I32)v1.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_wide(thread* t)
{
    t->is_wide |= 0x01;
    print_opcode("wide\n");
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifnull(thread* t)
{
    print_opcode("ifnull\n");
    StackVar v1=t->stack[t->rsp];
    t->rsp--;
    I16 off = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    if(v1.data==0) {
        t->pc += off;
    }
    else {
        t->pc +=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_ifnonnull(thread* t)
{
    print_opcode("ifnonnull\n");
    StackVar v1=t->stack[t->rsp];
    t->rsp--;
    I16 off = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    if(v1.data!=0) {
        t->pc += off;
    }
    else {
        t->pc +=2;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_goto_w(thread* t)
{
    print_opcode("goto_w\n");
    I32 offset = (((t->code->code[t->pc])<<24)
                |((t->code->code[t->pc+1])<<16)
                |((t->code->code[t->pc+2])<<8)
                |((t->code->code[t->pc+3])));
    t->pc += offset-1;
    return COLD_POINT_SUCCESS;
}
