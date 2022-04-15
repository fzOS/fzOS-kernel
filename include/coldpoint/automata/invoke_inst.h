#ifndef INVOKE_INST_H
#define INVOKE_INST_H
#include <coldpoint/threading/thread.h>
cpstatus opcode_invokespecial(thread* t);
cpstatus opcode_invokestatic(thread* t);
cpstatus opcode_invokevirtual(thread* t);
cpstatus opcode_invokeinterface(thread* t);
cpstatus opcode_return(thread* t);
cpstatus opcode_ireturn(thread* t);
cpstatus opcode_lreturn(thread* t);
cpstatus opcode_freturn(thread* t);
cpstatus opcode_dreturn(thread* t);
cpstatus opcode_areturn(thread* t);
void invoke_method(thread* t,class* target_class,CodeAttribute* code_attr,U64 param_count);
#endif
