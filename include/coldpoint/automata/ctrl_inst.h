#ifndef CTRL_INST
#define CTRL_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_goto(thread* t);
cpstatus opcode_tableswitch(thread* t);
cpstatus opcode_lookupswitch(thread* t);
cpstatus opcode_wide(thread* t);
cpstatus opcode_ifnull(thread* t);
cpstatus opcode_ifnonnull(thread* t);
cpstatus opcode_goto_w(thread* t);
#endif
