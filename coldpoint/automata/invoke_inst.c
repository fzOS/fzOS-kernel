#include <coldpoint/automata/invoke_inst.h>
#include <coldpoint/threading/thread.h>
#include <coldpoint/classloader.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <common/kstring.h>
#include <coldpoint/native/nativehandler.h>
static U64 get_param_count(const U8* in)
{
    in += 1;
    //skip (
    U64 result = 0;
    while(*in!=')') {
        switch (*in) {
            case 'L': {
                result++;
                while(*in!=';') {
                    in++;
                }
                break;
            }
            case '[': {
                break;
            }
            default: {
                result++;
                break;
            }
        }
        in++;
    }
    return result;
}
void invoke_method(thread* t,class* target_class,CodeAttribute* code_attr,U64 param_count)
{
    U64 new_stack_size_required = sizeof(stack_frame)/sizeof(StackVar);
    U64 new_rbp = t->rsp-param_count+1;
    t->rsp-=param_count;
    U64 new_rsp = new_rbp+new_stack_size_required;
    for(I64 i=param_count-1;i>=0;i--) {
        t->stack[new_rsp+i].data = t->stack[new_rbp+i].data;
        t->stack[new_rsp+i].type = t->stack[new_rbp+i].type;
        print_opcode("param %d:type %d,val %x,stack no %d->%d\n",i,t->stack[new_rsp+i].type,t->stack[new_rsp+i].data,new_rbp+i,new_rsp+i);
    }
    new_rsp += param_count;
    /*
    我们的JVM栈结构：
    (RBP位置)
    |-return thread status
    |-return class*(U64)
    |-return code*(U64)
    |-return pc(U64)
    |-return rsp(U64)
    |-return rbp(U64)
    |-local variables(U64*n)
    |-method stack(U64*m) <---(RSP位于0～m之间)
    (RBP+(5+n+m))
    由于定长栈设计，将会发生类型提升。
    i.e. 整数类型-> U64;
         浮点类型-> double;
    */
    t->stack[new_rbp  ].data = (U64)t->status;
    t->stack[new_rbp  ].type = STACK_TYPE_THREAD_STATUS;
    t->stack[new_rbp+1].data = (U64)t->class;
    t->stack[new_rbp+1].type = STACK_TYPE_CLASS_REF;
    t->stack[new_rbp+2].data = (U64)t->code;
    t->stack[new_rbp+2].type = STACK_TYPE_CODE_POINTER;
    t->stack[new_rbp+3].data = t->pc;
    t->stack[new_rbp+3].type = STACK_TYPE_PC;
    t->stack[new_rbp+4].data = t->rsp;
    t->stack[new_rbp+4].type = STACK_TYPE_RSP;
    t->stack[new_rbp+5].data = t->rbp;
    t->stack[new_rbp+5].type = STACK_TYPE_RBP;
    t->class = target_class;
    t->code = code_attr;
    t->pc = 0;
    t->status = (t->status==THREAD_RUNNING)?THREAD_RUNNING:THREAD_READY;
    t->rsp = new_rsp;
    t->rbp = new_rbp;
    return;
}
static void return_from_method(thread* t,int has_return_val)
{
    if(!t->rbp) {
        terminate_thread(t,EXIT_SUCCESS);
        return;
    }
    StackVar var = t->stack[t->rsp];
    U64 base = t->rbp;
    t->status= t->stack[base].data;
    t->class = (class*)t->stack[base+1].data;
    t->code  = (CodeAttribute*)t->stack[base+2].data;
    t->pc    = t->stack[base+3].data;
    t->rsp   = t->stack[base+4].data;
    t->rbp   = t->stack[base+5].data;
    if(has_return_val) {
        t->stack[++t->rsp] = var;
    }
    return;
}
cpstatus opcode_invokespecial(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    MethodRefConstant* ref = ((MethodRefConstant*)(t->class->buffer+t->class->constant_entry_offset)+no);
    class* target_class = get_class_by_index(t->class,ref->class_index);
    if(target_class==nullptr) {
        except(t," Target class not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,ref->name_and_type_index));
    const U8* target_name = class_get_utf8_string(t->class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(t->class,name_type_info->descriptor_index);
    print_opcode("invokespecial %s.%s->%s(%s)\n",target_class->class_name,target_name,target_desc,target_class->type==CLASS_KERNEL_API?"Kernel API":"Java API");
    if(target_class->type==CLASS_KERNEL_API) {
        NativeClass* native_class = (NativeClass*) target_class;
        return native_class->entry(t,target_name,target_desc,NATIVE_INVOKE);
    }
    MethodInfoEntry* method_info = get_method_by_name_and_type(target_class,target_name,target_desc);
    if(method_info==nullptr) {
        except(t,"Target method not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    U64 param_count = get_param_count(target_desc)+1;//This
    U64 code_name_index = class_get_utf8_string_index(target_class,(U8*)"Code");
    CodeAttribute* code = (CodeAttribute*)&target_class->buffer[class_get_method_attribute_by_name(target_class,method_info,code_name_index)->info_offset];
    invoke_method(t,target_class,code,param_count);

    return COLD_POINT_SUCCESS;
}
cpstatus opcode_invokestatic(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    MethodRefConstant* ref = ((MethodRefConstant*)(t->class->buffer+t->class->constant_entry_offset)+no);
    class* target_class = get_class_by_index(t->class,ref->class_index);
    if(target_class==nullptr) {
        except(t," Target class not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,ref->name_and_type_index));
    const U8* target_name = class_get_utf8_string(t->class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(t->class,name_type_info->descriptor_index);
    print_opcode("invokestatic %s->%s(%s)\n",target_name,target_desc,target_class->type==CLASS_KERNEL_API?"Kernel API":"Java API");
        if(target_class->type==CLASS_KERNEL_API) {
        NativeClass* native_class = (NativeClass*) target_class;
        return native_class->entry(t,target_name,target_desc,NATIVE_INVOKE);
    }
    MethodInfoEntry* method_info = get_method_by_name_and_type(target_class,target_name,target_desc);
    if(method_info==nullptr) {
        except(t,"Target method not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    U64 param_count = get_param_count(target_desc);
    U64 code_name_index = class_get_utf8_string_index(target_class,(U8*)"Code");
    CodeAttribute* code = (CodeAttribute*)&target_class->buffer[class_get_method_attribute_by_name(target_class,method_info,code_name_index)->info_offset];
    invoke_method(t,target_class,code,param_count);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_invokevirtual(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    MethodRefConstant* ref = ((MethodRefConstant*)(t->class->buffer+t->class->constant_entry_offset)+no);
    class* target_class = get_class_by_index(t->class,ref->class_index);
    if(target_class==nullptr) {
        except(t," Target class not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,ref->name_and_type_index));
    const U8* target_name = class_get_utf8_string(t->class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(t->class,name_type_info->descriptor_index);
    print_opcode("invokevirtual %s->%s(%s)\n",target_name,target_desc,target_class->type==CLASS_KERNEL_API?"Kernel API":"Java API");
    if(target_class->type==CLASS_KERNEL_API) {
        NativeClass* native_class = (NativeClass*) target_class;
        return native_class->entry(t,target_name,target_desc,NATIVE_INVOKE);
    }
    //Dynamically linking.
    U64 param_count = get_param_count(target_desc);
    object* o = (object*) t->stack[t->rsp-param_count].data;
    CodeAttribute* code=nullptr;
    for(int i=0;i<o->var_count;i++) {
        if(!strcomp((const char*)target_desc,o->var[i].typename)
        && !strcomp((const char*)target_name,o->var[i].signature)) {
            code = (CodeAttribute*)o->var->value;
        }
    }
    if(code==nullptr) {
        except(t,"Target method not found");
    }
    invoke_method(t,target_class,code,param_count+1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_invokeinterface(thread* t)
{
    //What is an interface?
    return opcode_invokevirtual(t);
}
cpstatus opcode_ireturn(thread* t)
{
    print_opcode("ireturn\n");
    return_from_method(t,1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lreturn(thread* t)
{
    print_opcode("lreturn\n");
    return_from_method(t,1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_freturn(thread* t)
{
    print_opcode("freturn\n");
    return_from_method(t,1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dreturn(thread* t)
{
    print_opcode("dreturn\n");
    return_from_method(t,1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_areturn(thread* t)
{
    print_opcode("areturn\n");
    return_from_method(t,1);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_return(thread* t)
{
    print_opcode("return\n");
    return_from_method(t,0);
    return COLD_POINT_SUCCESS;
}
