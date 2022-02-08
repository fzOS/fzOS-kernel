#ifndef CTRL_INST
#define CTRL_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_goto(thread* t);
cpstatus opcode_tableswitch(thread* t);
cpstatus opcode_lookupswitch(thread* t);
#endif
