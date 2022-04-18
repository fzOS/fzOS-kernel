#include <coldpoint/native/native_file.h>
#include <coldpoint/native/native_typewrapper.h>
#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <memory/memory.h>
const char * const g_file_class_name  = "fzos/util/File";
const char * const g_file_desc_class_name  = "fzos/util/FileDescriptor";
static NativeClassInlineLinkedListNode g_file_class_linked_node = {
    .c.class_name = (const U8*)g_file_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = file_class_entry
};
cpstatus native_file_open(thread* t);
cpstatus native_file_read(thread* t);
cpstatus native_file_seek(thread* t);
cpstatus native_file_get_desc(thread* t);
static NativeMethod g_file_methods[] = {
    {
        (U8*)"<init>",
        (U8*)"(Ljava/lang/String;)V",
        native_file_open
    },
    {
        (U8*)"read",
        (U8*)"([BJ)J",
        native_file_read
    },
    {
        (U8*)"seek",
        (U8*)"(J)J",
        native_file_seek
    },
    {
        (U8*)"getDescriptor",
        (U8*)"()Lfzos/util/FileDescriptor;",
        native_file_get_desc
    }
};
cpstatus native_file_open(thread* t)
{
    NativeTypeWrapperObject* str = (NativeTypeWrapperObject*)t->stack[t->rsp--].data;
    FileObject* o = (FileObject*)t->stack[t->rsp--].data;
    if(generic_open((char*)get_constant_from_string(str),&o->f)!=FzOS_SUCCESS) {
        except(t,"File not found");
        return COLD_POINT_EXEC_FAILURE;
    }
    set_parameter_in_object(o->descriptor,"fileSize","J",g_file_desc_class_name,o->f.size);
    set_parameter_in_object(o->descriptor,"offset","J",g_file_desc_class_name,0);
    return COLD_POINT_SUCCESS;
}
cpstatus native_file_read(thread* t)
{
    I64 len = t->stack[t->rsp--].data;
    Array* a = (Array*)t->stack[t->rsp--].data;
    FileObject* o = (FileObject*)t->stack[t->rsp--].data;
    if(len > a->length) {
        except(t,"Buffer size exceed");
        return COLD_POINT_EXEC_FAILURE;
    }
    len = o->f.filesystem->read(&o->f,a->value,len);
    set_parameter_in_object(o->descriptor,"offset","J",g_file_desc_class_name,o->f.offset);
    t->stack[++t->rsp].type = STACK_TYPE_LONG;
    t->stack[t->rsp].data   = len;
    return COLD_POINT_SUCCESS;
}
cpstatus native_file_seek(thread* t)
{
    I64 delta = t->stack[t->rsp--].data;
    FileObject* o = (FileObject*)t->stack[t->rsp--].data;
    o->f.filesystem->seek(&o->f,delta,SEEK_FROM_BEGINNING);
    set_parameter_in_object(o->descriptor,"offset","J",g_file_desc_class_name,o->f.offset);
    t->stack[++t->rsp].type = STACK_TYPE_LONG;
    t->stack[t->rsp].data   = o->f.offset;
    return COLD_POINT_SUCCESS;
}
cpstatus native_file_get_desc(thread* t)
{
    FileObject* o = (FileObject*)t->stack[t->rsp].data;
    t->stack[t->rsp].data = (U64)o->descriptor;
    return COLD_POINT_SUCCESS;
}
cpstatus file_class_entry(thread* t,const U8* name,const U8* desc,NativeClassOperations operations)
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
            for(U64 i=0;i<sizeof(g_file_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_file_methods[i].name)
                && !strcomp((char*)desc,(char*)g_file_methods[i].desc)) {
                    return g_file_methods[i].method(t);
                }
            }
            except(t,"Native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_NEW: {
            FileObject* obj = allocate_heap(sizeof(FileObject));
            class* c = getclass((const U8*)g_file_desc_class_name);
            obj->descriptor = new_object(c);
            obj->descriptor->parent_class = (class*)&g_file_class_linked_node.c;
            t->stack[++t->rsp].data = (U64)obj;
            t->stack[t->rsp].type = STACK_TYPE_REFERENCE;
            print_opcode("File Object:%x\n",obj);
            return COLD_POINT_SUCCESS;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus file_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_file_class_linked_node.node,-1);
    file file;
    int ret = generic_open("/FileDescriptor.class",&file);
    if(ret !=FzOS_SUCCESS) {
        printk(" Open fzos.util.FileDescriptor fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    void* buf = memalloc(file.size);
    ret = file.filesystem->read(&file,buf,file.size);
    if(ret==0) {
        memfree(buf);
        printk(" Read FileDescriptor fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    loadclass(buf);
    memfree(buf);
    return COLD_POINT_SUCCESS;
}
