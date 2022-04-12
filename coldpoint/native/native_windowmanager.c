#include <coldpoint/native/native_windowmanager.h>
#include <common/kstring.h>
#include <coldpoint/native/native_typewrapper.h>
const char * const g_windowmanager_class_name  = "fzos/gui/WindowManager";
const char * const g_window_class_name  = "fzos/gui/Window";
static NativeClassInlineLinkedListNode g_windowmanager_class_linked_node = {
    .c.class_name = (const U8*)g_windowmanager_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = windowmanager_class_entry
};
cpstatus native_windowmanager_enter_graphical_mode(thread* t);
cpstatus native_windowmanager_exit_graphical_mode(thread* t);
cpstatus native_windowmanager_composite(thread* t);
cpstatus native_windowmanager_create_window(thread* t);
cpstatus native_windowmanager_destroy_window(thread* t);
cpstatus native_windowmanager_set_window_event(thread* t);
static NativeMethod g_windowmanager_methods[] = {
    {
        (U8*)"composite",
        (U8*)"()V",
        native_windowmanager_composite
    },
    {
        (U8*)"createWindow",
        (U8*)"(IIIIILjava/lang/String;)Lfzos/gui/Window",
        native_windowmanager_create_window
    },
    {
        (U8*)"destroyWindow",
        (U8*)"(Lfzos/gui/Window;)V",
        native_windowmanager_destroy_window
    },
    {
        (U8*)"enterGraphicalMode",
        (U8*)"()V",
        native_windowmanager_enter_graphical_mode
    },
    {
        (U8*)"exitGraphicalMode",
        (U8*)"()V",
        native_windowmanager_exit_graphical_mode
    },
    {
        (U8*)"setWindowEvent",
        (U8*)"(Lfzos/gui/Window;Lfzos/gui/WindowEvent;)V",
        native_windowmanager_set_window_event
    },
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
        case NATIVE_GETFIELD: {
            Window* w = ((WindowObject*)t->stack[t->rsp--].data)->w;
            //A lot of it......
            /*
                public int windowMode;
                public int windowStatus;
                public String windowTitle;
                public int windowWidth,windowHeight,windowPositionX,windowPositionY;
                public int[] imageData;
                public WindowEvent event;
            */
            if(!strcomp((char*)name,"imageData")) {
                t->stack[++t->rsp].data = (U64)&(w->buffer);
                t->stack[t->rsp].type   = STACK_TYPE_REFERENCE;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowMode")) {
                t->stack[++t->rsp].data = w->mode;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowStatus")) {
                t->stack[++t->rsp].data = w->status;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowTitle")) {
                t->stack[++t->rsp].data = (U64)constant_to_string((U8*)w->caption);
                t->stack[t->rsp].type   = STACK_TYPE_REFERENCE;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowWidth")) {
                t->stack[++t->rsp].data = w->width;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowHeight")) {
                t->stack[++t->rsp].data = w->height;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowPositionX")) {
                t->stack[++t->rsp].data = w->x;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowPositionY")) {
                t->stack[++t->rsp].data = w->y;
                t->stack[t->rsp].type   = STACK_TYPE_INT;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"event")) {
                t->stack[++t->rsp].data = (U64)w->event_receiver;
                t->stack[t->rsp].type   = STACK_TYPE_REFERENCE;
                return COLD_POINT_SUCCESS;
            }
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_PUTFIELD: {
            U64 val = t->stack[t->rsp--].data;
            Window* w = ((WindowObject*)t->stack[t->rsp--].data)->w;
            //A lot of it......
            /*
                public int windowMode;
                public int windowStatus;
                public String windowTitle;
                public int windowWidth,windowHeight,windowPositionX,windowPositionY;
                public int[] imageData;
                public WindowEvent event;
            */
            if(!strcomp((char*)name,"windowMode")) {
                w->mode = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowStatus")) {
                w->status = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowTitle")) {
                w->caption = (char*)get_constant_from_string((NativeTypeWrapperObject*)val);
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowWidth")) {
                w->width = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowHeight")) {
                w->height = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowPositionX")) {
                w->x = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"windowPositionY")) {
                w->y = val&0xFFFFFFFF;
                return COLD_POINT_SUCCESS;
            }
            if(!strcomp((char*)name,"event")) {
                w->event_receiver = (object*)val;
                return COLD_POINT_SUCCESS;
            }
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
            for(U64 i=0;i<sizeof(g_windowmanager_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_windowmanager_methods[i].name)
                && !strcomp((char*)type,(char*)g_windowmanager_methods[i].desc)) {
                    return g_windowmanager_methods[i].method(t);
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
cpstatus native_windowmanager_enter_graphical_mode(thread* t)
{
    enter_graphical_mode();
    return COLD_POINT_SUCCESS;
}
cpstatus native_windowmanager_exit_graphical_mode(thread* t)
{
    exit_graphical_mode();
    return COLD_POINT_SUCCESS;
}
cpstatus native_windowmanager_composite(thread* t)
{
    composite();
    return COLD_POINT_SUCCESS;
}
cpstatus native_windowmanager_create_window(thread* t)
{
    char* title = (char*)get_constant_from_string((NativeTypeWrapperObject*)t->stack[t->rsp--].data);
    int pos_y   = (t->stack[t->rsp--].data)&0xFFFFFFFF;
    int pos_x   = (t->stack[t->rsp--].data)&0xFFFFFFFF;
    int height  = (t->stack[t->rsp--].data)&0xFFFFFFFF;
    int width   = (t->stack[t->rsp--].data)&0xFFFFFFFF;
    int attrs   = (t->stack[t->rsp--].data)&0xFFFFFFFF;
    Window* w   = create_window(pos_x,pos_y,width,height,title,attrs,nullptr,t);
    WindowObject* o = allocate_heap(sizeof(WindowObject));
    o->o.parent_class = (class*)&g_window_class_linked_node.c;
    o->w = w;
    t->stack[++t->rsp].data = (U64)o;
    t->stack[t->rsp].type   = STACK_TYPE_REFERENCE;
    return COLD_POINT_SUCCESS;
}
cpstatus native_windowmanager_destroy_window(thread* t)
{
    //TODO:Destroy Window.
    return COLD_POINT_SUCCESS;
}
cpstatus native_windowmanager_set_window_event(thread* t)
{
    object* o       = (object*)t->stack[t->rsp--].data;
    WindowObject* w = (WindowObject*)t->stack[t->rsp--].data;
    w->w->event_receiver = o;
    register_window_callbacks(w->w);
    return COLD_POINT_SUCCESS;
}
cpstatus windowmanager_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_windowmanager_class_linked_node.node,-1);
    insert_existing_inline_node(loaded_class_list,&g_window_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
