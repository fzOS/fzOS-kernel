#include <zcrystal/window_manager.h>
#include <zcrystal/window_console.h>
#include <drivers/fbcon.h>
#include <memory/memory.h>
#include <drivers/vmsvga.h>
static U64 g_next_window_id;
InlineLinkedList g_window_linked_list = {
    .tail = &g_window_linked_list.head
};
FzOSResult enter_graphical_mode(void)
{
    //First,we disable console.
    //hook_window_console();
    //Then,we clear the whole screen.
    graphics_clear_screen(DEFAULT_BACKGROUND_COLOR);
    //After that, We create the debug window.
    create_window(100,100,600,400,"Debug Log",nullptr);
    composite();
    return FzOS_SUCCESS;
}
FzOSResult exit_graphical_mode(void)
{
    restore_window_console();
    graphics_clear_screen(DEFAULT_CONSOLE_BACKGROUND_COLOR);
    return FzOS_SUCCESS;
}
Window* create_window(U32 x,U32 y,U32 width,U32 height,char* title,object* event_receiver)
{
    U64 size_needed = sizeof(WindowInlineLinkedListNode)+width*(height+WINDOW_CAPTION_HEIGHT)*sizeof(U32);
    WindowInlineLinkedListNode* node = memalloc(size_needed);
    node->w.caption = title;
    node->w.buffer.length = (height+WINDOW_CAPTION_HEIGHT)*width*sizeof(U8);
    node->w.buffer.type = (const U8*)"B";
    node->w.x = x;
    node->w.y = y;
    node->w.width = width;
    node->w.height = height;
    node->w.event_receiver = event_receiver;
    node->w.window_id = g_next_window_id++;
    //clear to white.
    memset(node->w.buffer.value,0xFF,width*(height+WINDOW_CAPTION_HEIGHT)*sizeof(U32));
    update_window_caption(&node->w,1);
    insert_existing_inline_node(&g_window_linked_list,&node->node,-1);
    return &node->w;
}
void update_window_caption(Window* w,U8 activity)
{
    U32* pixeldata = (U32*)w->buffer.value;
    for(int i=0;i<WINDOW_CAPTION_HEIGHT;i++) {
        for(int j=0;j<w->width;j++) {
            pixeldata[i*w->width+j] = activity?WINDOW_CAPTION_COLOR_ACTIVE:WINDOW_CAPTION_COLOR_INACTIVE;
        }
    }
    //TODO:Draw String.
}
void composite(void)
{
    printk("Start window composition.\n");
    U32* screen_buffer = g_graphics_data.frame_buffer_base;
    WindowInlineLinkedListNode* node = (WindowInlineLinkedListNode*)g_window_linked_list.head.next;
    while(node!=nullptr) {
        //TODO:check visibility.
        Window* w = &node->w;
        printk("Compositing window %d.\n",w->window_id);
        U32* window_buffer = (U32*)w->buffer.value;
        int composite_area_left   = (w->x<0)?(-w->x):0;
        int composite_area_right  = ((w->x+w->width)>g_graphics_data.pixels_per_line)?
                                    (g_graphics_data.pixels_vertical-w->x):
                                    w->width;
        int composite_area_top    = (w->y<0)?(-w->y):0;
        int composite_area_bottom = ((w->y+w->height+WINDOW_CAPTION_HEIGHT)>g_graphics_data.pixels_vertical)?
                                    (g_graphics_data.pixels_vertical-w->y):
                                    w->height+WINDOW_CAPTION_HEIGHT;
        int screen_offset_x = w->x>0?w->x:0;
        int screen_offset_y = w->y>0?w->y:0;
        printk("Area:%d*%d@(%d,%d)\n",composite_area_right-composite_area_left,composite_area_bottom-composite_area_top,screen_offset_x,screen_offset_y);
        for(int i=composite_area_top;i<composite_area_bottom;i++) {
            memcpy(screen_buffer+(screen_offset_y+i)*g_graphics_data.pixels_per_line+screen_offset_x,
                   window_buffer+i*w->width+composite_area_left,
                   (composite_area_right-composite_area_left)*sizeof(U32));
        }
        node = (WindowInlineLinkedListNode*)node->node.next;
    }
    printk("End window composition.\n");
    g_screen_dirty = 1;
}
