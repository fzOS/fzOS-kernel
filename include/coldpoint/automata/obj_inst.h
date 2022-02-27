#ifndef OBJ_INST
#define OBJ_INST
#include <coldpoint/threading/thread.h>
cpstatus opcode_ldc(thread* t);
cpstatus opcode_ldc2_w(thread* t);
cpstatus opcode_new(thread* t);
cpstatus opcode_getstatic(thread* t);
cpstatus opcode_putstatic(thread* t);
cpstatus opcode_invokespecial(thread* t);
cpstatus opcode_getfield(thread* t);
cpstatus opcode_putfield(thread* t);
cpstatus opcode_instanceof(thread* t);
cpstatus opcode_checkcast(thread* t);
#endif
