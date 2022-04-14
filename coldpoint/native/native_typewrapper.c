#include <coldpoint/native/native_typewrapper.h>
#include <common/kstring.h>
#include <memory/memory.h>
const char * const g_integer_class_name  = "java/lang/Integer";
const char * const g_character_class_name  = "java/lang/Character";
const char * const g_byte_class_name  = "java/lang/Byte";
const char * const g_short_class_name  = "java/lang/Short";
const char * const g_long_class_name  = "java/lang/Long";
const char * const g_float_class_name  = "java/lang/Float";
const char * const g_double_class_name  = "java/lang/Double";
const char * const g_string_class_name  = "java/lang/String";
const char * const g_integer_value_desc = "(I)Ljava/lang/Integer;";
const char * const g_character_value_desc = "(C)Ljava/lang/Character;";
const char * const g_byte_value_desc = "(B)Ljava/lang/Byte;";
const char * const g_short_value_desc = "(S)Ljava/lang/Short;";
const char * const g_long_value_desc = "(J)Ljava/lang/Long;";
const char * const g_float_value_desc = "(F)Ljava/lang/Float;";
const char * const g_double_value_desc = "(D)Ljava/lang/Double;";
static NativeClassInlineLinkedListNode g_integer_class_linked_node = {
    .c.class_name = (const U8*)g_integer_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_character_class_linked_node = {
    .c.class_name = (const U8*)g_character_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_byte_class_linked_node = {
    .c.class_name = (const U8*)g_byte_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_short_class_linked_node = {
    .c.class_name = (const U8*)g_short_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_long_class_linked_node = {
    .c.class_name = (const U8*)g_long_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_float_class_linked_node = {
    .c.class_name = (const U8*)g_float_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_double_class_linked_node = {
    .c.class_name = (const U8*)g_double_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
static NativeClassInlineLinkedListNode g_string_class_linked_node = {
    .c.class_name = (const U8*)g_string_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = typewrapper_class_entry
};
cpstatus typewrapper_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_integer_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_character_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_byte_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_short_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_long_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_string_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_float_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_double_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
cpstatus typewrapper_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations)
{
    switch (operations) {
        case NATIVE_GETSTATIC: case NATIVE_PUTSTATIC: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_GETFIELD: case NATIVE_PUTFIELD: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_INVOKE: {
            StackVar t1 = t->stack[t->rsp];
            if(!strcomp((char*)name,"valueOf")) {
                NativeTypeWrapperObject* o = allocate_heap(sizeof(NativeTypeWrapperObject));
                o->o.var_count             = 0;
                o->val = t1.data;
                //valueOf.
                if(!strcomp((char*)type,g_integer_value_desc)) {
                    o->o.parent_class = (class*)&g_integer_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_character_value_desc)) {
                    o->o.parent_class = (class*)&g_character_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_byte_value_desc)) {
                    o->o.parent_class = (class*)&g_byte_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_short_value_desc)) {
                    o->o.parent_class = (class*)&g_short_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_long_value_desc)) {
                    o->o.parent_class = (class*)&g_long_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_float_value_desc)) {
                    o->o.parent_class = (class*)&g_float_class_linked_node.c;
                }
                else if(!strcomp((char*)type,g_double_value_desc)) {
                    o->o.parent_class = (class*)&g_double_class_linked_node.c;
                }
                t1.data = (U64)o;
                t1.type = STACK_TYPE_REFERENCE;
            }
            else if(!strcomp((char*)name,"<init>")) {
                //String.
                Array* a = (Array*)t1.data;
                NativeTypeWrapperObject* o = (NativeTypeWrapperObject*)t->stack[--t->rsp].data;
                strcopy((char*)o->val,(char*)a->value,a->length+1);
                return COLD_POINT_SUCCESS;
            }
            else {
                //getValue.
                t1.data = ((NativeTypeWrapperObject*)t1.data)->val;
                if(!strcomp((char*)name,"intValue")) {
                    t1.type = STACK_TYPE_INT;
                }
                else if(!strcomp((char*)name,"charValue")) {
                    t1.type = STACK_TYPE_CHAR;
                }
                else if(!strcomp((char*)name,"byteValue")) {
                    t1.type = STACK_TYPE_BYTE;
                }
                else if(!strcomp((char*)name,"shortValue")) {
                    t1.type = STACK_TYPE_SHORT;
                }
                else if(!strcomp((char*)name,"longValue")) {
                    t1.type = STACK_TYPE_LONG;
                }
                else if(!strcomp((char*)name,"floatValue")) {
                    t1.type = STACK_TYPE_FLOAT;
                }
                else if(!strcomp((char*)name,"doubleValue")) {
                    t1.type = STACK_TYPE_DOUBLE;
                }
            }
            t->stack[t->rsp] = t1;
            return COLD_POINT_SUCCESS;
        }
        case NATIVE_NEW: {
            if(!strcomp((char*)name,"java/lang/String")) {
                int len = t->stack[t->rsp].data;
                NativeTypeWrapperObject* str = allocate_heap(sizeof(NativeTypeWrapperObject));
                str->val = (U64)allocate_heap(len);
                str->o.parent_class = (class*)&g_string_class_linked_node.c;
                str->o.var_count    = 0;
                t->stack[t->rsp].data = (U64)str;
                return COLD_POINT_SUCCESS;
            }
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    return COLD_POINT_SUCCESS;
}
NativeTypeWrapperObject* constant_to_string(const U8* in)
{
    NativeTypeWrapperObject* o = allocate_heap(sizeof(NativeTypeWrapperObject));
    o->val = (U64)in;
    o->o.parent_class = (class*)&g_string_class_linked_node.c;
    o->o.var_count    = 0;
    return o;
}
const U8* get_constant_from_string(const NativeTypeWrapperObject* o)
{
    return (const U8*)o->val;
}
