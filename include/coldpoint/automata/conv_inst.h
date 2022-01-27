#ifndef CONV_INST
#define CONV_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_to_long(thread* t);
cpstatus opcode_to_int(thread* t);
cpstatus opcode_to_float(thread* t);
cpstatus opcode_to_double(thread* t);
#endif
