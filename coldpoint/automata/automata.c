#include <coldpoint/automata/automata.h>
#include <coldpoint/automata/const_inst.h>
#include <coldpoint/automata/load_inst.h>
#include <coldpoint/automata/stack_inst.h>
#include <coldpoint/automata/math_inst.h>
#include <coldpoint/automata/conv_inst.h>
#include <coldpoint/automata/comp_inst.h>
#include <coldpoint/automata/ctrl_inst.h>
cpstatus (*g_automata_opcode[256])(thread* c)= {
    opcode_nop,opcode_aconst_null,opcode_iconst_m1,opcode_iconst_0,//0x00~0x03
    opcode_iconst_1,opcode_iconst_2,opcode_iconst_3,opcode_iconst_4,//0x04~0x07
    opcode_iconst_5,opcode_lconst_0,opcode_lconst_1,opcode_fconst_0,//0x08~0x0b
    opcode_fconst_1,opcode_fconst_2,opcode_dconst_0,opcode_dconst_1,//0x0c~0x0f
    opcode_bipush,opcode_sipush,nullptr,nullptr,//0x10~0x13
    nullptr,opcode_load,opcode_load,opcode_load,//0x14~0x17
    opcode_load,opcode_load,opcode_load0,opcode_load1,//0x18~0x1b
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x1c~0x1f
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x20~0x23
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x24~0x27
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x28~0x2b
    opcode_load2,opcode_load3,nullptr,nullptr,//0x2c~0x2f
    nullptr,nullptr,nullptr,nullptr,//0x30~0x33
    nullptr,nullptr,opcode_store,opcode_store,//0x34~0x37
    opcode_store,opcode_store,opcode_store,opcode_store0,//0x38~0x3b,
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x3c~0x3f
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x40~0x43
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x44~0x47
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x48~0x4b
    opcode_store1,opcode_store2,opcode_store3,nullptr,//0x4c~0x4f
    nullptr,nullptr,nullptr,nullptr,//0x50~0x53
    nullptr,nullptr,nullptr,opcode_pop,//0x54~0x57
    opcode_pop2,opcode_dup,opcode_dup_x1,opcode_dup_x2,//0x58~0x5b
    opcode_dup2,opcode_dup2_x1,opcode_dup2_x2,opcode_swap,//0x5c~0x5f
    opcode_add,opcode_add,opcode_add,opcode_add,//0x60~0x63
    opcode_sub,opcode_sub,opcode_sub,opcode_sub,//0x64~0x67
    opcode_mul,opcode_mul,opcode_mul,opcode_mul,//0x68~0x6b
    opcode_div,opcode_div,opcode_div,opcode_div,//0x6c~0x6f
    opcode_mod,opcode_mod,opcode_mod,opcode_mod,//0x70~0x73
    opcode_neg,opcode_neg,opcode_neg,opcode_neg,//0x74~0x77
    opcode_shl,opcode_shl,opcode_shr,opcode_shr,//0x78~0x7b
    opcode_ushr,opcode_ushr,opcode_and,opcode_and,//0x7c~0x7f
    opcode_or,opcode_or,opcode_xor,opcode_xor,//0x80~0x83
    opcode_inc,opcode_to_long,opcode_to_float,opcode_to_double,//0x84~0x87
    opcode_to_int,opcode_to_float,opcode_to_double,opcode_to_int,//0x88~0x8b
    opcode_to_long,opcode_to_double,opcode_to_int,opcode_to_long,//0x8c~0x8f
    opcode_to_float,opcode_int_to_byte,opcode_int_to_char,opcode_int_to_short,//0x90~0x93
    opcode_lcmp,opcode_cmpl,opcode_cmpg,opcode_cmpl,//0x94~0x97
    opcode_cmpg,opcode_ifeq,opcode_ifne,opcode_iflt,//0x98~0x9b
    opcode_ifge,opcode_ifgt,opcode_ifle,opcode_ificmpeq,//0x9c~0x9f
    opcode_ificmpne,opcode_ificmplt,opcode_ificmpge,opcode_ificmpgt,//0xa0~0xa3
    opcode_ificmple,opcode_ifacmpeq,opcode_ifacmpne,opcode_goto,//0xa4~0xa7,
    nullptr,nullptr,opcode_tableswitch,opcode_lookupswitch,//0xa8~0xab

};
void except(thread* t,char* msg)
{
    printk(" Exception caught at %x :%s.\n",t->pc,msg);
}
void automata_main_loop(thread* t)
{
    while(t->status!=THREAD_TERMINATED) { //TODO:Multi-threading.
        print_opcode("%d ",t->rsp);
        g_automata_opcode[t->code->code[t->pc++]](t);
    }
}
