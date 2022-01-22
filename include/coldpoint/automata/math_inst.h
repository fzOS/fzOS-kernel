#ifndef MATH_INST
#define MATH_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_add(thread* t);
cpstatus opcode_sub(thread* t);
cpstatus opcode_mul(thread* t);
cpstatus opcode_div(thread* t);
cpstatus opcode_mod(thread* t);
cpstatus opcode_neg(thread* t);
#endif
