#ifndef CONST_INST
#define CONST_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_nop(thread* t);
cpstatus opcode_aconst_null(thread* t);
cpstatus opcode_dconst_0(thread* t);
cpstatus opcode_dconst_1(thread* t);
cpstatus opcode_fconst_0(thread* t);
cpstatus opcode_fconst_1(thread* t);
cpstatus opcode_fconst_2(thread* t);
cpstatus opcode_iconst_m1(thread* t);
cpstatus opcode_iconst_0(thread* t);
cpstatus opcode_iconst_1(thread* t);
cpstatus opcode_iconst_2(thread* t);
cpstatus opcode_iconst_3(thread* t);
cpstatus opcode_iconst_4(thread* t);
cpstatus opcode_iconst_5(thread* t);
cpstatus opcode_lconst_0(thread* t);
cpstatus opcode_lconst_1(thread* t);
cpstatus opcode_bipush(thread* t);
cpstatus opcode_sipush(thread* t);
#endif
