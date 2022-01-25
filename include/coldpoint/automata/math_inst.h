#ifndef MATH_INST
#define MATH_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_add(thread* t);
cpstatus opcode_sub(thread* t);
cpstatus opcode_mul(thread* t);
cpstatus opcode_div(thread* t);
cpstatus opcode_mod(thread* t);
cpstatus opcode_neg(thread* t);
cpstatus opcode_shl(thread* t);
cpstatus opcode_shr(thread* t);
cpstatus opcode_ushr(thread* t);
cpstatus opcode_and(thread* t);
cpstatus opcode_or(thread* t);
cpstatus opcode_xor(thread* t);
cpstatus opcode_inc(thread* t);
#endif
