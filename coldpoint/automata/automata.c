#include <coldpoint/automata/automata.h>
#include <coldpoint/automata/const_inst.h>
#include <coldpoint/automata/load_inst.h>
#include <coldpoint/automata/stack_inst.h>
#include <coldpoint/automata/math_inst.h>
#include <coldpoint/automata/conv_inst.h>
#include <coldpoint/automata/comp_inst.h>
#include <coldpoint/automata/ctrl_inst.h>
#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/automata/invoke_inst.h>
cpstatus invalid_opcode(thread* t)
{
    except(t,"invalid opcode");
    return COLD_POINT_EXEC_FAILURE;
}
cpstatus (*g_automata_opcode[256])(thread* c)= {
    opcode_nop,opcode_aconst_null,opcode_iconst_m1,opcode_iconst_0,//0x00~0x03
    opcode_iconst_1,opcode_iconst_2,opcode_iconst_3,opcode_iconst_4,//0x04~0x07
    opcode_iconst_5,opcode_lconst_0,opcode_lconst_1,opcode_fconst_0,//0x08~0x0b
    opcode_fconst_1,opcode_fconst_2,opcode_dconst_0,opcode_dconst_1,//0x0c~0x0f
    opcode_bipush,opcode_sipush,opcode_ldc,opcode_ldc,//0x10~0x13
    opcode_ldc2_w,opcode_load,opcode_load,opcode_load,//0x14~0x17
    opcode_load,opcode_load,opcode_load0,opcode_load1,//0x18~0x1b
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x1c~0x1f
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x20~0x23
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x24~0x27
    opcode_load2,opcode_load3,opcode_load0,opcode_load1,//0x28~0x2b
    opcode_load2,opcode_load3,opcode_iaload,opcode_laload,//0x2c~0x2f
    opcode_faload,opcode_daload,opcode_aaload,opcode_baload,//0x30~0x33
    opcode_caload,opcode_saload,opcode_store,opcode_store,//0x34~0x37
    opcode_store,opcode_store,opcode_store,opcode_store0,//0x38~0x3b,
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x3c~0x3f
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x40~0x43
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x44~0x47
    opcode_store1,opcode_store2,opcode_store3,opcode_store0,//0x48~0x4b
    opcode_store1,opcode_store2,opcode_store3,opcode_iastore,//0x4c~0x4f
    opcode_lastore,opcode_fastore,opcode_dastore,opcode_aastore,//0x50~0x53
    opcode_bastore,opcode_castore,opcode_sastore,opcode_pop,//0x54~0x57
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
    invalid_opcode,invalid_opcode,opcode_tableswitch,opcode_lookupswitch,//0xa8~0xab
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xac~0xaf
    invalid_opcode,opcode_return,opcode_getstatic,opcode_putstatic,//0xb0~0xb3
    opcode_getfield,opcode_putfield,opcode_invokevirtual,opcode_invokespecial,//0xb4~0xb7
    opcode_invokestatic,opcode_invokeinterface,invalid_opcode,opcode_new,//0xb8~0xbb
    opcode_newarray,opcode_anewarray,opcode_arraylength,invalid_opcode,//0xbc~0xbf
    opcode_checkcast,opcode_instanceof,invalid_opcode,invalid_opcode,//0xc0~0xc3
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xc4~0xc7
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xc8~0xcb
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xcc~0xcf
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xd0~0xd3
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xd4~0xd7
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xd8~0xdb
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xdc~0xdf
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xe0~0xe3
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xe4~0xe7
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xe8~0xeb
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xec~0xef
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xf0~0xf3
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xf4~0xf7
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xf8~0xfb
    invalid_opcode,invalid_opcode,invalid_opcode,invalid_opcode,//0xfc~0xff
};
void except(thread* t,char* msg)
{
    printk("\n Exception caught at %x :%s.\n",t->pc,msg);
    t->status = THREAD_TERMINATED;
}
void automata_main_loop(thread* t)
{
    while(t->status!=THREAD_TERMINATED) { //TODO:Multi-threading.
        print_opcode(" %d %d ",t->pc,t->rsp);
        g_automata_opcode[t->code->code[t->pc++]](t);
    }
}
