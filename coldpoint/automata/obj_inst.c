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
