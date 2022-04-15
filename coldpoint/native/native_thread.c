#include <coldpoint/native/native_thread.h>
#include <coldpoint/classloader.h>
#include <coldpoint/heap/heap.h>
#include <common/file.h>
#include <memory/memory.h>
#include <common/kstring.h>
static const char* const g_native_thread_class_name = "fzos/threading/Thread";
static const char* const g_native_thread_manager_class_name = "fzos/threading/ThreadManager";
static const char* const g_native_thread_method_type = "(Lfzos/threading/Thread;)V";
cpstatus native_register_thread(thread* t);
cpstatus native_start_thread(thread* t);
cpstatus native_suspend_thread(thread* t);
cpstatus native_destroy_thread(thread* t);
static NativeClassInlineLinkedListNode g_native_thread_class_linked_node = {
    .c.class_name = (const U8*)g_native_thread_manager_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = thread_class_entry
};
static NativeMethod g_thread_methods[] = {
    {
        (U8*)"registerThread",
        (U8*)g_native_thread_method_type,
        native_register_thread
    },
    {
        (U8*)"startThread",
        (U8*)g_native_thread_method_type,
        native_start_thread
    },
    {
        (U8*)"suspendThread",
        (U8*)g_native_thread_method_type,
        native_suspend_thread
    },
    {
        (U8*)"destroyThread",
        (U8*)g_native_thread_method_type,
        native_destroy_thread
    }
};
cpstatus native_register_thread(thread* t)
{
    object* java_thread_object = (object*)t->stack[t->rsp--].data;
    class* c = java_thread_object->parent_class;
    MethodInfoEntry* main = class_get_method_by_name_and_desc(c, class_get_utf8_string_index(c,(U8*)"run"), class_get_utf8_string_index(c,(U8*)"()I"));
    U64 code_name_index = class_get_utf8_string_index(c,(U8*)"Code");
    if(main==nullptr) {
        except(t,"Cannot load public int run().");
        return COLD_POINT_EXEC_FAILURE;
    }
    CodeAttribute* code = (CodeAttribute*)&c->buffer[class_get_method_attribute_by_name(c,main,code_name_index)->info_offset];
    thread* new_thread = create_thread(g_current_thread->t.process,code,c,g_current_thread->t.console);
    set_parameter_in_object(java_thread_object,"threadId","I",g_native_thread_class_name,new_thread->tid);
    //Emulate parameter "this"
    new_thread->stack[sizeof(stack_frame)/sizeof(StackVar)].data = (U64)java_thread_object;
    new_thread->stack[sizeof(stack_frame)/sizeof(StackVar)].type = STACK_TYPE_REFERENCE;
    return COLD_POINT_SUCCESS;
}
cpstatus native_start_thread(thread* t)
{
    object* java_thread_object = (object*)t->stack[t->rsp--].data;
    U64 tid = get_parameter_in_object(java_thread_object,"threadId","I",g_native_thread_class_name);
    thread* target = get_thread_by_tid(tid);
    if(target == nullptr) {
        except(t,"Target thread not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    target->status = THREAD_READY;
    set_parameter_in_object(java_thread_object,"threadStatus","I",g_native_thread_class_name,THREAD_READY);
    return COLD_POINT_SUCCESS;
}
cpstatus native_suspend_thread(thread* t)
{
    object* java_thread_object = (object*)t->stack[t->rsp--].data;
    U64 tid = get_parameter_in_object(java_thread_object,"threadId","I",g_native_thread_class_name);
    thread* target = get_thread_by_tid(tid);
    if(target == nullptr) {
        except(t,"Target thread not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    target->status = THREAD_BLOCKED;
    set_parameter_in_object(java_thread_object,"threadStatus","I",g_native_thread_class_name,THREAD_BLOCKED);
    return COLD_POINT_SUCCESS;
}
cpstatus native_destroy_thread(thread* t)
{
    object* java_thread_object = (object*)t->stack[t->rsp--].data;
    U64 tid = get_parameter_in_object(java_thread_object,"threadId","I",g_native_thread_class_name);
    thread* target = get_thread_by_tid(tid);
    if(target == nullptr) {
        except(t,"Target thread not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    destroy_thread(target);
    set_parameter_in_object(java_thread_object,"threadStatus","I",g_native_thread_class_name,THREAD_TERMINATED);
    return COLD_POINT_SUCCESS;
}
cpstatus thread_class_entry(thread* t, const U8* name, const U8* desc, NativeClassOperations operations)
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
            for(U64 i=0;i<sizeof(g_thread_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_thread_methods[i].name)
                && !strcomp((char*)desc,(char*)g_thread_methods[i].desc)) {
                    return g_thread_methods[i].method(t);
                }
            }
            except(t,"native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;

        }
        case NATIVE_NEW: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus thread_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_native_thread_class_linked_node.node,-1);
    file file;
    int ret = generic_open("/Thread.class",&file);
    if(ret !=FzOS_SUCCESS) {
        printk(" Open fzos.threading.Thread fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    void* buf = memalloc(file.size);
    ret = file.filesystem->read(&file,buf,file.size);
    if(ret==0) {
        memfree(buf);
        printk(" Read Thread fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    loadclass(buf);
    memfree(buf);
    return COLD_POINT_SUCCESS;
}
