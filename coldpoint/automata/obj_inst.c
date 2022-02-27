#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <coldpoint/automata/automata.h>
static inline ConstantEntry* get_const_entry_by_index(class* c,int no)
{
    return ((ConstantEntry*)(c->buffer+c->constant_entry_offset))+no;
}
static inline class* get_class_by_index(class* c,int no)
{
    const U8* target_class_name = class_get_utf8_string(c,((ClassInfoConstant*)get_const_entry_by_index(c,no))->name_index);
    return getclass(target_class_name);
}
static inline FieldInfoEntry* get_field_by_name_and_type_index(class* c,const U8* name,const U8* type)
{
    FieldInfoEntry* field_entry = (FieldInfoEntry*)&(c->buffer[c->fields_pool_entry_offset]);
    if(c->fields_pool_entry_count) {
        for(int i=0;i<c->fields_pool_entry_count;i++) {
            if(!strcomp((char*)name,(char*)class_get_utf8_string(c,field_entry[i].name_index))
             &&!strcomp((char*)type,(char*)class_get_utf8_string(c,field_entry[i].descriptor_index))) {
                return field_entry;
            }
        }
    }
    return nullptr;
}
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
cpstatus opcode_invokespecial(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc+=2;
    print_opcode("invokespecial #%d\n",no);


    return COLD_POINT_SUCCESS;
}
cpstatus opcode_getstatic(thread* t)
{
    U32 no;
    no = ((t->code->code[t->pc])<<8|t->code->code[t->pc+1]);
    t->pc +=2;
    print_opcode("getstatic #%d ",no);
    StackVar v1;
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    class* target_class = get_class_by_index(t->class,field->class_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(target_class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(target_class,name_type_info->descriptor_index);
    FieldInfoEntry* field_info = get_field_by_name_and_type_index(target_class,target_name,target_desc);
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
    print_opcode("putstatic #%d ",no);
    StackVar v1 = t->stack[(t->rsp)--];
    FieldRefConstant* field = (FieldRefConstant*)get_const_entry_by_index(t->class,no);
    class* target_class = get_class_by_index(t->class,field->class_index);
    NameAndTypeInfoConstant* name_type_info = ((NameAndTypeInfoConstant*)get_const_entry_by_index(t->class,field->name_and_type_index));
    const U8* target_name = class_get_utf8_string(target_class,name_type_info->name_index);
    const U8* target_desc = class_get_utf8_string(target_class,name_type_info->descriptor_index);
    FieldInfoEntry* field_info = get_field_by_name_and_type_index(target_class,target_name,target_desc);
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
    print_opcode("putfield #%d ",no);
    StackVar v2 = t->stack[t->rsp],v1 = t->stack[t->rsp-1];
    t->rsp -=2;
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
    print_opcode("getfield #%d ",no);
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
