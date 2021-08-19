#ifndef LOAD_INST
#define LOAD_INST
#include <coldpoint/automata/automata.h>
cpstatus opcode_load(thread* t);
cpstatus opcode_load0(thread* t);
cpstatus opcode_load1(thread* t);
cpstatus opcode_load2(thread* t);
cpstatus opcode_load3(thread* t);

cpstatus opcode_store(thread* t);
cpstatus opcode_store0(thread* t);
cpstatus opcode_store1(thread* t);
cpstatus opcode_store2(thread* t);
cpstatus opcode_store3(thread* t);

#endif
