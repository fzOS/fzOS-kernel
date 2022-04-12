#ifndef INVOKE_INST_H
#define INVOKE_INST_H
#include <coldpoint/threading/thread.h>
cpstatus opcode_invokespecial(thread* t);
cpstatus opcode_invokestatic(thread* t);
cpstatus opcode_invokevirtual(thread* t);
cpstatus opcode_invokeinterface(thread* t);
cpstatus opcode_return(thread* t);
void invoke_method(thread* t,class* target_class,CodeAttribute* code_attr,U64 param_count);
#endif
