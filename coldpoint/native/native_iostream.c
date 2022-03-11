#include <coldpoint/native/native_iostream.h>
#include <coldpoint/common/class.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/automata/obj_inst.h>
#include <drivers/console.h>
#include <memory/memory.h>
#include <common/kstring.h>
const char * const g_iostream_class_name  = "fzos/base/IOStream";
cpstatus native_iostream_printf(thread* t);
static NativeMethod g_iostream_methods[] = {
    {
        (U8*)"printf",
        (U8*)"(Ljava/lang/String;[Ljava/lang/Object;)V",
        native_iostream_printf
    }
};

cpstatus native_iostream_printf(thread* t)
{
    print_opcode("Got native call printf().\n");
    //(Ljava/lang/String;[Ljava/lang/Object;)V
    //Get String.
    object* o = (object*)t->stack[t->rsp-1].data;
    t->rsp-=2;
    ObjectVar* var = o->var;
    const char* val = nullptr;
    for(U64 i=0;i<o->var_count;i++) {
        if(!strcomp(var[i].typename,"[B") && !strcomp(var[i].signature,"internBytes")) {
            val = (char*)((Array*)(var[i].value))->value;
        }
    }
    if(val!=nullptr) {
        printk(val);
    }
    else {
        except(t,"No string found");
        return COLD_POINT_EXEC_FAILURE;
    }
    return COLD_POINT_SUCCESS;
}
static NativeClassInlineLinkedListNode g_iostream_class_linked_node = {
    .c.class_name = (const U8*)g_iostream_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = iostream_class_entry
};

typedef struct {
    object o;
    Console* con;
} IOStreamObject;
cpstatus iostream_class_entry(thread* t,const U8* name,const U8* desc,NativeClassOperations operations)
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
            for(U64 i=0;i<sizeof(g_iostream_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_iostream_methods[i].name)
                && !strcomp((char*)desc,(char*)g_iostream_methods[i].desc)) {
                    return g_iostream_methods[i].method(t);
                }
            }
            except(t,"native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_NEW: {
            IOStreamObject* obj = memalloc(sizeof(IOStreamObject));
            obj->o.parent_class = (class*)&g_iostream_class_linked_node.c;
            obj->o.var_count    = 0;
            t->stack[++t->rsp].data = (U64)obj;
            t->stack[t->rsp].type = STACK_TYPE_REFERENCE;
            return COLD_POINT_SUCCESS;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus iostream_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_iostream_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
