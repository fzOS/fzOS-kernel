#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <coldpoint/automata/automata.h>
#include <memory/memory.h>
cpstatus opcode_ldc2_w(thread* t)
{
    t->is_wide |= 0x1;
    return opcode_ldc(t);
}
cpstatus opcode_ldc(thread* t)
{
    U32 no;
    if(t->is_wide&0x1) {
        no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
        t->pc+=2;
        t->is_wide&=(~0x1);
    }
    else {
        no = t->code->code[t->pc];
        t->pc++;
    }
    print_opcode("ldc #%d:",no);
    ConstantEntry* const_entry = ((ConstantEntry*)(t->class->buffer+t->class->constant_entry_offset))+no;
    StackVar v2;
    double *p1 = (double*)(&(v2.data));
    switch(const_entry->type) {
        case CONSTANT_INT: {
            v2.data = ((IntegerInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_INT;
            print_opcode("type=int,val=%d\n",((IntegerInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_FLOAT: {
            *p1 = ((FloatInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_FLOAT;
            print_opcode("type=float,val=%f\n",((FloatInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_LONG: {
            v2.data = ((LongInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_LONG;
            print_opcode("type=long,val=%lld\n",((LongInfoConstant*)const_entry)->val);
            break;
        }
        case CONSTANT_DOUBLE: {
            *p1 = ((DoubleInfoConstant*)const_entry)->val;
            v2.type = STACK_TYPE_DOUBLE;
            print_opcode("type=double,val=%lf\n",((DoubleInfoConstant*)const_entry)->val);
            break;
        }
        default: {
            print_opcode("Unknown type:%d.\n",const_entry->type);
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    t->stack[++(t->rsp)] = v2;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_new(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    print_opcode("new #%d:",no);
    ConstantEntry* const_entry = ((ConstantEntry*)(t->class->buffer+t->class->constant_entry_offset))+no;
    StackVar v2;
    v2.type = STACK_TYPE_REFERENCE;
    const U8* class_name = class_get_utf8_string(t->class,((ClassInfoConstant*)const_entry)->name_index);
    v2.data = (U64)new_object(getclass(class_name));
    t->stack[++(t->rsp)] = v2;
    print_opcode("%s:0x%x\n",class_name,v2.data);
    return COLD_POINT_SUCCESS;
}
U64 newarray_real(thread* t,U64 length,const U8* type)
{
    Array* a = allocate_heap(sizeof(Array)+length*sizeof(U64));
    memset(a,0x00,sizeof(Array)+length*sizeof(U64));
    a->type = type;
    a->length = length;
    return (U64)a;
}
cpstatus opcode_newarray(thread* t)
{
    ArrayType type = t->code->code[t->pc];
    t->pc++;
    StackVar v1 = t->stack[t->rsp];
    U32 length = v1.data;
    const char* type_name = "";
    if(length>0) {
        switch(type) {
            case T_BOOLEAN: {
                type_name = "[Z";
                length = (length-1) / (sizeof(U64)/sizeof(byte))+1;
                break;
            }
            case T_CHAR: {
                //in Java,char is 16 BITS!!!!
                type_name = "[C";
                length = (length-1) / (sizeof(U64)/sizeof(short))+1;
                break;
            }
            case T_BYTE: {
                type_name = "[B";
                length = (length-1) / (sizeof(U64)/sizeof(byte))+1;
                break;
            }
            case T_SHORT: {
                type_name = "[S";
                length = (length-1) / (sizeof(U64)/sizeof(short))+1;
                break;
            }
            case T_INT:  {
                type_name = "[I";
                length = (length-1) / (sizeof(U64)/sizeof(int))+1;
                break;
            }
            case T_FLOAT: {
                type_name = "[F";
                length = (length-1) / (sizeof(U64)/sizeof(float))+1;
                break;
            }
            case T_LONG: {
                type_name = "[J";
                break;
            }
            case T_DOUBLE: {
                type_name = "[D";
                break;
            }
            default: {
                break;
            }
        }
    }
    U64 addr = newarray_real(t,length,(const U8*)type_name);
    v1.type = STACK_TYPE_REFERENCE;
    v1.data = addr;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_anewarray(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    StackVar v1 = t->stack[t->rsp];
    U64 length = v1.data;
    const U8* class_name = class_get_utf8_string(t->class,class_get_class_name_index(t->class,no));
    U64 address = newarray_real(t,length,class_name);
    v1.data = address;
    v1.type = STACK_TYPE_REFERENCE;
    t->stack[t->rsp] = v1;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_getstatic(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    StackVar v1;
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    class* target_class = get_class_by_index(t->class,field->class_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(t->class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(t->class,name_type_info->descriptor_index);
    print_opcode("getstatic %s -> %s ",target_name,target_desc);
    FieldInfoEntry* field_info = get_field_by_name_and_type(target_class,target_name,target_desc);
    if(field_info==nullptr) {
        except(t,"No field found.");
        return COLD_POINT_EXEC_FAILURE;
    }
    if(!(field_info->access_flags&ACCESS_STATIC)) {
        except(t,"Not a static value.");
        return COLD_POINT_EXEC_FAILURE;
    }
    v1.data = field_info->val;
    switch(target_desc[0]) {
        case 'Z':case 'B':case 'C':case 'S':case 'I': {
            v1.type = STACK_TYPE_INT;
            break;
        }
        case 'F': {
            v1.type = STACK_TYPE_FLOAT;
            break;
        }
        case 'D': {
            v1.type = STACK_TYPE_DOUBLE;
            break;
        }
        case 'J': {
            v1.type = STACK_TYPE_LONG;
            break;
        }
        case 'L':case '[': {
            v1.type = STACK_TYPE_REFERENCE;
            break;
        }
        default: {
            except(t,"Unknown type.");
            return COLD_POINT_EXEC_FAILURE;
        }
    }
    t->stack[++(t->rsp)]=v1;
    print_opcode("%s->%s ==> %d\n",target_name,target_desc,field_info->val);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_putstatic(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    StackVar v1 = t->stack[(t->rsp)--];
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    class* target_class = get_class_by_index(t->class,field->class_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(t->class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(t->class,name_type_info->descriptor_index);
    print_opcode("putstatic %s -> %s ",target_name,target_desc);
    FieldInfoEntry* field_info = get_field_by_name_and_type(target_class,target_name,target_desc);
    if(field_info==nullptr) {
        except(t,"No field found.");
        return COLD_POINT_EXEC_FAILURE;
    }
    if(!(field_info->access_flags&ACCESS_STATIC)) {
        except(t,"Not a static value.");
        return COLD_POINT_EXEC_FAILURE;
    }
    if((field_info->access_flags&ACCESS_FINAL)) {
        except(t,"Final values should not be assigned during method.");
        return COLD_POINT_EXEC_FAILURE;
    }
    field_info->val = v1.data;
    print_opcode("%s->%s <== %d\n",target_name,target_desc,v1.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_putfield(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    StackVar v2 = t->stack[t->rsp],v1 = t->stack[t->rsp-1];
    t->rsp -=2;
    object* obj = (object*)v1.data;
    class* c = t->class;
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    const U8* target_class_name = class_get_utf8_string(c,((ClassInfoConstant*)get_const_entry_by_index(c,field->class_index))->name_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(c,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(c,name_type_info->descriptor_index);
    print_opcode("putfield %s -> %s ",target_name,target_desc);
    ObjectVar* vars = obj->var;
    int found=0;
    for(int i=0;i<obj->var_count;i++) {
        if(!strcomp(vars[i].class,(char*)target_class_name)
         &&!strcomp(vars[i].signature,(char*)target_name)
         &&!strcomp(vars[i].typename,(char*)target_desc)) {
            vars[i].value = v2.data;
            found = 1;
            break;

        }
    }
    if(!found) {
        except(t,"Field not found.");
        return COLD_POINT_EXEC_FAILURE;
    }
    print_opcode("%s@%s->%s <== %d\n",target_name,target_class_name,target_desc,v2.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_getfield(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    StackVar v1 = t->stack[t->rsp];
    object* obj = (object*)v1.data;
    class* c = t->class;
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    const U8* target_class_name = class_get_utf8_string(c,((ClassInfoConstant*)get_const_entry_by_index(c,field->class_index))->name_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(c,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(c,name_type_info->descriptor_index);
    print_opcode("getfield %s -> %s ",target_name,target_desc);
    ObjectVar* vars = obj->var;
    int found=0;
    for(int i=0;i<obj->var_count;i++) {
        if(!strcomp(vars[i].class,(char*)target_class_name)
         &&!strcomp(vars[i].signature,(char*)target_name)
         &&!strcomp(vars[i].typename,(char*)target_desc)) {
            v1.data = vars[i].value;
            found = 1;
            break;

        }
    }
    if(!found) {
        except(t,"Field not found.");
        return COLD_POINT_EXEC_FAILURE;
    }
    switch(target_desc[0]) {
        case 'Z':case 'B':case 'C':case 'S':case 'I': {
            v1.type = STACK_TYPE_INT;
            break;
        }
        case 'F': {
            v1.type = STACK_TYPE_FLOAT;
            break;
        }
        case 'D': {
            v1.type = STACK_TYPE_DOUBLE;
            break;
        }
        case 'J': {
            v1.type = STACK_TYPE_LONG;
            break;
        }
        case 'L':case '[': {
            v1.type = STACK_TYPE_REFERENCE;
            break;
        }
        default: {
            except(t,"Unknown type.");
            return COLD_POINT_EXEC_FAILURE;
        }
    }
    t->stack[t->rsp]=v1;
    print_opcode("%s@%s->%s ==> %d\n",target_name,target_class_name,target_desc,v1.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_checkcast(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    print_opcode("checkcast #%d \n",no);
    const U8* target_class_name = class_get_utf8_string(t->class,((ClassInfoConstant*)get_const_entry_by_index(t->class,no))->name_index);
    StackVar v1 = t->stack[t->rsp];
    object* obj = (object*)v1.data;
    if(obj!=nullptr) {
        class* current = obj->parent_class;
        U8 found=0;
        while(1) {
            if(!strcomp((char*)current->class_name,(char*)target_class_name)) {
                found=1;
                break;
            }
            current = getclass(class_get_utf8_string(current,class_get_class_name_index(current,current->super_class)));
            if(current == nullptr) {
                break;
            }
        }
        if(!found) {
            except(t,"Invalid cast.");
            return COLD_POINT_EXEC_FAILURE;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_instanceof(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    const U8* target_class_name = class_get_utf8_string(t->class,((ClassInfoConstant*)get_const_entry_by_index(t->class,no))->name_index);
    StackVar v1 = t->stack[t->rsp];
    v1.type = STACK_TYPE_INT;
    object* obj = (object*)v1.data;
    v1.data = 0;
    if(obj!=nullptr) {
        class* current = obj->parent_class;
        while(1) {
            if(!strcomp((char*)current->class_name,(char*)target_class_name)) {
                v1.data = 1;
                break;
            }
            current = getclass(class_get_utf8_string(current,class_get_class_name_index(current,current->super_class)));
            if(current == nullptr) {
                break;
            }
        }
    }
    t->stack[t->rsp] = v1;
    print_opcode("instanceof #%d :%d\n",no,v1.data);
    return COLD_POINT_SUCCESS;
}
