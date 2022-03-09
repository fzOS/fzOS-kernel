#include <coldpoint/native/native_iostream.h>
#include <coldpoint/common/class.h>
const char * const g_iostream_class_name  = "fzos/base/IOStream";


static NativeClassInlineLinkedListNode g_iostream_class_linked_node = {
    .c.class_name = (const U8*)g_iostream_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = iostream_class_entry
};
cpstatus iostream_class_entry(thread* t,const U8* name,const U8* type,NativeClassOperations operations)
{
    switch (operations) {
        case NATIVE_GETSTATIC: case NATIVE_PUTSTATIC: {

            break;
        }
        case NATIVE_GETFIELD: case NATIVE_PUTFIELD: {

            break;
        }
        case NATIVE_INVOKE : {

            break;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus iostream_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_iostream_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
