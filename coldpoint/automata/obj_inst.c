#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <coldpoint/automata/automata.h>
#include <memory/memory.h>
#include <coldpoint/native/native_typewrapper.h>
Array* newarray_real(thread* t,U64 length,const U8* type);
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
        case CONSTANT_STRING: {
            const U8* val = class_get_utf8_string(t->class,((StringInfoConstant*)const_entry)->string_index);
            NativeTypeWrapperObject* string_obj = constant_to_string(val);
            v2.data = (U64)string_obj;
            v2.type = STACK_TYPE_REFERENCE;
            print_opcode("type=string\n");
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
    ConstantEntry* const_entry = ((ConstantEntry*)(t->class->buffer+t->class->constant_entry_offset))+no;
    StackVar v2;
    v2.type = STACK_TYPE_REFERENCE;
    const U8* class_name = class_get_utf8_string(t->class,((ClassInfoConstant*)const_entry)->name_index);
    print_opcode("new %s:",class_name);
    class* target_class = getclass(class_name);
    if(target_class!=nullptr&&target_class->type==CLASS_KERNEL_API) {
        print_opcode("(Kernel API)\n");
        NativeClass* native_class = (NativeClass*) target_class;
        return native_class->entry(t,class_name,nullptr,NATIVE_NEW);
    }
    print_opcode("(Java API)\n");
    v2.data = (U64)new_object(target_class);
    t->stack[++(t->rsp)] = v2;
    print_opcode("%s:0x%x\n",class_name,v2.data);
    return COLD_POINT_SUCCESS;
}
Array* newarray_real(thread* t,U64 length,const U8* type)
{
    Array* a = allocate_heap(sizeof(Array)+length*sizeof(U64));
    memset(a,0x00,sizeof(Array)+length*sizeof(U64));
    a->type = type;
    a->length = length;
    return a;
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
    Array* addr = newarray_real(t,length,(const U8*)type_name);
    addr->length = v1.data;
    v1.type = STACK_TYPE_REFERENCE;
    v1.data = (U64)addr;
    t->stack[t->rsp] = v1;
    print_opcode("newarray %s:%x\n",type_name,addr);
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
    Array* address = newarray_real(t,length,class_name);
    v1.data = (U64)address;
    v1.type = STACK_TYPE_REFERENCE;
    t->stack[t->rsp] = v1;
    print_opcode("anewarray %s:%x\n",class_name,address);
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
    print_opcode("getfield %s@%s->%s ==> %d\n",target_name,target_class_name,target_desc,v1.data);
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
cpstatus aload_internal(thread* t,U64 data_width)
{
    U64 index = t->stack[t->rsp].data;
    StackVar ref = t->stack[t->rsp-1];
    t->rsp -= 1;
    Array* arr = (Array*) ref.data;
    if(arr==nullptr) {
        except(t,"Null array address");
        return COLD_POINT_EXEC_FAILURE;
    }
    //Check index.
    if(index>arr->length) {
        except(t,"Index overflow");
        return COLD_POINT_EXEC_FAILURE;
    }
    U64 group_data = arr->value[index/(sizeof(U64)/data_width)];
    U64 result = index % (sizeof(U64)/data_width);
    switch(data_width) {
        case 1: {
            result = (group_data>>(result*8))&0xFF;
            break;
        }
        case 2: {
            result = (group_data>>(result*16))&0xFFFF;
            break;
        }
        case 4: {
            result = (group_data>>(result*32))&0xFFFFFFFF;
            break;
        }
        case 8: {
            result = group_data;
            break;
        }
    }
    ref.data = result;
    t->stack[t->rsp] = ref;
    print_opcode("i/l/f/d/a/b/c/s aload:%d <== %d\n",result,index);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iaload(thread* t)
{
    U64 result = aload_internal(t,sizeof(int));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_INT;
    return result;
}
cpstatus opcode_laload(thread* t)
{
    U64 result = aload_internal(t,sizeof(long long));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_LONG;
    return result;
}
cpstatus opcode_faload(thread* t)
{
    U64 result = aload_internal(t,sizeof(float));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_FLOAT;
    return result;
}
cpstatus opcode_daload(thread* t)
{
    U64 result = aload_internal(t,sizeof(double));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_DOUBLE;
    return result;
}
cpstatus opcode_aaload(thread* t)
{
    U64 result = aload_internal(t,sizeof(U64));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_REFERENCE;
    return result;
}
cpstatus opcode_baload(thread* t)
{
    U64 result = aload_internal(t,sizeof(char));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_BYTE;
    return result;
}
cpstatus opcode_caload(thread* t)
{
    U64 result = aload_internal(t,sizeof(short));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_CHAR;
    return result;
}
cpstatus opcode_saload(thread* t)
{
    U64 result = aload_internal(t,sizeof(short));
    if(result==COLD_POINT_SUCCESS)
        t->stack[t->rsp].type = STACK_TYPE_SHORT;
    return result;
}
cpstatus astore_internal(thread* t,U64 data_width)
{
    StackVar value=t->stack[t->rsp],index=t->stack[t->rsp-1],ref=t->stack[t->rsp-2];
    t->rsp -= 3;
    Array* arr = (Array*) ref.data;
    if(arr==nullptr) {
        except(t,"Null array address");
        return COLD_POINT_EXEC_FAILURE;
    }
    //Check index.
    if(index.data>arr->length) {
        except(t,"Index overflow");
        return COLD_POINT_EXEC_FAILURE;
    }
    U64 group_data = arr->value[index.data/(sizeof(U64)/data_width)];
    U64 result = index.data % (sizeof(U64)/data_width);
    switch(data_width) {
        case 1: {
            group_data |= (value.data&0xFF)<<(result*8);
            break;
        }
        case 2: {
            group_data |= (value.data&0xFFFF)<<(result*16);
            break;
        }
        case 4: {
            group_data |= (value.data&0xFFFFFFFF)<<(result*32);
            break;
        }
        case 8: {
            group_data = value.data;
            break;
        }
    }
    arr->value[index.data/(sizeof(U64)/data_width)] = group_data;
    print_opcode("i/l/f/d/a/b/c/s astore:%d ==> %d\n",index.data,value.data);
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iastore(thread* t)
{
    return astore_internal(t,sizeof(int));
}
cpstatus opcode_lastore(thread* t)
{
    return astore_internal(t,sizeof(long long));
}
cpstatus opcode_fastore(thread* t)
{
    return astore_internal(t,sizeof(float));
}
cpstatus opcode_dastore(thread* t)
{
    return astore_internal(t,sizeof(double));
}
cpstatus opcode_aastore(thread* t)
{
    return astore_internal(t,sizeof(U64));
}
cpstatus opcode_bastore(thread* t)
{
    return astore_internal(t,sizeof(char));
}
cpstatus opcode_castore(thread* t)
{
    return astore_internal(t,sizeof(short));
}
cpstatus opcode_sastore(thread* t)
{
    return astore_internal(t,sizeof(short));
}
cpstatus opcode_arraylength(thread* t)
{
    StackVar ref = t->stack[t->rsp];
    Array* arr = (Array*) ref.data;
    if(arr==nullptr) {
        except(t,"Null array address");
        t->rsp -= 1;
        return COLD_POINT_EXEC_FAILURE;
    }
    ref.data = arr->length;
    ref.type = STACK_TYPE_INT;
    t->stack[t->rsp] = ref;
    print_opcode("arraylength:%d\n",ref.data);
    return COLD_POINT_SUCCESS;
}
