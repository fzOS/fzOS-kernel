#include <coldpoint/native/native_windowmanager.h>
#include <common/kstring.h>
const char * const g_windowmanager_class_name  = "fzos/gui/WindowManager";
const char * const g_window_class_name  = "fzos/gui/Window";
static NativeClassInlineLinkedListNode g_windowmanager_class_linked_node = {
    .c.class_name = (const U8*)g_windowmanager_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = windowmanager_class_entry
};
static NativeClassInlineLinkedListNode g_window_class_linked_node = {
    .c.class_name = (const U8*)g_window_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = window_class_entry
};
cpstatus native_window_show(thread* t);
cpstatus native_window_hide(thread* t);
static NativeMethod g_window_methods[] = {
    {
        (U8*)"show",
        (U8*)"()V",
        native_window_show
    },
    {
        (U8*)"hide",
        (U8*)"()V",
        native_window_hide
    }
};
cpstatus native_window_show(thread* t)
{
    WindowObject* o = (WindowObject*)t->stack[t->rsp--].data;
    o->w->status &= (~WINDOW_STATUS_INACTIVE);
    composite();
    return COLD_POINT_SUCCESS;
}
cpstatus native_window_hide(thread* t)
{
    WindowObject* o = (WindowObject*)t->stack[t->rsp--].data;
    o->w->status |= WINDOW_STATUS_INACTIVE;
    composite();
    return COLD_POINT_SUCCESS;
}
cpstatus window_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations)
{
switch (operations) {
        case NATIVE_GETSTATIC: case NATIVE_PUTSTATIC: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_GETFIELD: case NATIVE_PUTFIELD: {
            //A lot of it......
            /*
                public int windowMode;
                public int windowStatus;
                public String windowTitle;
                public int windowWidth,windowHeight,windowPositionX,windowPositionY;
                public int[] imageData;
                public WindowEvent event;
            */

            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_INVOKE: {
            for(U64 i=0;i<sizeof(g_window_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_window_methods[i].name)
                && !strcomp((char*)type,(char*)g_window_methods[i].desc)) {
                    return g_window_methods[i].method(t);
                }
            }
            except(t,"Native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_NEW: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    return COLD_POINT_SUCCESS;
}

cpstatus windowmanager_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations)
{

}
cpstatus windowmanager_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_windowmanager_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_window_class_linked_node.node,-1);
}
