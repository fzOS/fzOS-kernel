#include <coldpoint/automata/automata.h>
#include <coldpoint/automata/const_inst.h>
cpstatus (*automata_opcode[256])(thread* c)= {
    opcode_nop,opcode_aconst_null,opcode_iconst_m1,opcode_iconst_0,//0x00~0x03
    opcode_iconst_1,opcode_iconst_2,opcode_iconst_3,opcode_iconst_4,//0x04~0x07
    opcode_iconst_5,opcode_lconst_0,opcode_lconst_1,opcode_fconst_0,//0x08~0x0b
    opcode_fconst_1,opcode_fconst_2,opcode_dconst_0,opcode_dconst_1,//0x0c~0x0f
    opcode_bipush,opcode_sipush
};
