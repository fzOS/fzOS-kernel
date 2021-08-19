#ifndef STACK_INST
#define STACK_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_pop(thread* t);
cpstatus opcode_pop2(thread* t);
cpstatus opcode_dup(thread* t);
cpstatus opcode_dup_x1(thread* t);
cpstatus opcode_dup_x2(thread* t);
cpstatus opcode_dup2(thread* t);
cpstatus opcode_dup2_x1(thread* t);
cpstatus opcode_dup2_x2(thread* t);
cpstatus opcode_swap(thread* t);
#endif
