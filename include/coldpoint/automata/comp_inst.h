#ifndef COMP_INST
#define COMP_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_lcmp(thread* t);
cpstatus opcode_cmpg(thread* t);
cpstatus opcode_cmpl(thread* t);
cpstatus opcode_ifeq(thread* t);
cpstatus opcode_ifne(thread* t);
cpstatus opcode_iflt(thread* t);
cpstatus opcode_ifle(thread* t);
cpstatus opcode_ifgt(thread* t);
cpstatus opcode_ifge(thread* t);
cpstatus opcode_ificmpeq(thread* t);
cpstatus opcode_ificmpne(thread* t);
cpstatus opcode_ificmplt(thread* t);
cpstatus opcode_ificmple(thread* t);
cpstatus opcode_ificmpgt(thread* t);
cpstatus opcode_ificmpge(thread* t);
cpstatus opcode_ifacmpeq(thread* t);
cpstatus opcode_ifacmpne(thread* t);

#endif
