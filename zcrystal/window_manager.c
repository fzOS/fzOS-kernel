#include <zcrystal/window_manager.h>
#include <zcrystal/window_console.h>
#include <zcrystal/window_decoration.h>
#include <drivers/asciifont.h>
#include <drivers/fbcon.h>
#include <memory/memory.h>
#include <drivers/vmsvga.h>
#include <common/kstring.h>
#include <coldpoint/automata/invoke_inst.h>
static U64 g_next_window_id;
volatile int g_screen_lock = 0;
static const char* const window_event_names[] = {
    "onResize",
    "onClick",
    "onMove",
    "onMinimize",
    "onClose",
    "onActivate",
    "onInactivate"
};
static const char* const window_event_types[] = {
    "(II)V",
    "(II)V",
    "(IIII)V",
    "()V",
    "()V",
    "()V",
    "()V"
};
_Static_assert(sizeof(window_event_names)==sizeof(window_event_types),"Window event Names & Types don't match!");
void render_glyph_font(U32* buffer,char c,U32 glyph_color,U32 background_color)
{
    const unsigned char* orig_font = &FONTDATA_8x16[c*16];
    for(int j=0;j<WINDOW_TITLE_GLYPH_HEIGHT;j++) {
        for(int i=0;i<WINDOW_TITLE_GLYPH_WIDTH;i++) {
            buffer[j*WINDOW_TITLE_GLYPH_WIDTH+i] = (orig_font[(int)(j/1.5)]&(1<<(8-(int)(i/1.5))))?
                                                      glyph_color:
                                                      background_color;
        }
    }
}
InlineLinkedList g_window_linked_list = {
    .tail = &g_window_linked_list.head
};
void register_window_callbacks(Window* w)
{
    class* target_class = w->event_receiver->parent_class;
    CodeAttribute** attributes[] = {
        &w->code_on_resize,
        &w->code_on_click,
        &w->code_on_move,
        &w->code_on_minimize,
        &w->code_on_close,
        &w->code_on_activate,
        &w->code_on_inactivate
    };
    _Static_assert(sizeof(attributes)==sizeof(window_event_types),"Window event Names & Types don't match!");
    for(int i=0;i<sizeof(window_event_names)/sizeof(char*);i++) {
        MethodInfoEntry* method_info = get_method_by_name_and_type(target_class,
                                                                   (U8*)window_event_names[i],
                                                                   (U8*)window_event_types[i]);
        if(method_info==nullptr) {
            continue;
        }
        U64 code_name_index = class_get_utf8_string_index(target_class,(U8*)"Code");
        CodeAttribute* code = (CodeAttribute*)&target_class->buffer[class_get_method_attribute_by_name(target_class,method_info,code_name_index)->info_offset];
        *attributes[i] = code;
        printk("%s:%x\n",window_event_names[i],code);
    }
}
FzOSResult enter_graphical_mode(void)
{
    //First,we disable console.
    hook_window_console();
    //Then,we clear the whole screen.
    graphics_clear_screen(DEFAULT_BACKGROUND_COLOR);
    //After that, We create the debug window.
    Window* w = create_window(900,50,400,800,"Debug Log",WINDOW_MODE_NORMAL|WINDOW_MODE_NO_CLOSE,nullptr,nullptr);
    set_window_console_window(w,0xFFFFFFFF,0x3a6fa6);
    composite();
    return FzOS_SUCCESS;
}
FzOSResult exit_graphical_mode(void)
{
    restore_window_console();
    graphics_clear_screen(DEFAULT_CONSOLE_BACKGROUND_COLOR);
    return FzOS_SUCCESS;
}
Window* create_window(U32 x,U32 y,U32 width,U32 height,char* title,int attr,object* event_receiver,thread* ui_thread)
{
    U64 size_needed = sizeof(WindowInlineLinkedListNode)+width*(height+WINDOW_CAPTION_HEIGHT)*sizeof(U32);
    WindowInlineLinkedListNode* node = memalloc(size_needed);
    memset(&node->w,0x00,sizeof(Window));
    node->w.mode = attr;
    node->w.caption = title;
    node->w.buffer.length = (height+WINDOW_CAPTION_HEIGHT)*width*sizeof(U8);
    node->w.buffer.type = (const U8*)"B";
    node->w.status = WINDOW_STATUS_INACTIVE;
    node->w.x = x;
    node->w.y = y;
    node->w.width = width;
    node->w.height = height;
    node->w.event_receiver = event_receiver;
    node->w.window_id = g_next_window_id++;
    node->w.ui_thread = ui_thread;
    //clear to white.
    memset(node->w.buffer.value,0xFF,width*(height+WINDOW_CAPTION_HEIGHT)*sizeof(U32));
    update_window_caption(&node->w,0);
    if(event_receiver!=nullptr) {
        register_window_callbacks(&node->w);
    }
    insert_existing_inline_node(&g_window_linked_list,&node->node,-1);
    return &node->w;
}
void resize_window(Window* w,U32 width,U32 height)
{
    WindowInlineLinkedListNode* node = (WindowInlineLinkedListNode*)g_window_linked_list.tail;
    while(node != (WindowInlineLinkedListNode*)g_window_linked_list.head.next->prev) {
        if(&node->w==w) {
            int orig_width  = node->w.width;
            int copy_width = node->w.width>width?width:node->w.width;
            int copy_height= node->w.height>height?height:node->w.height;
            U64 size_needed = sizeof(WindowInlineLinkedListNode)+width*(height+WINDOW_CAPTION_HEIGHT)*sizeof(U32);
            //Allocate a new Window.
            WindowInlineLinkedListNode* new_node = memalloc(size_needed);
            memcpy(&new_node->w,w,sizeof(Window));
            new_node->w.width  = width;
            new_node->w.height = height;
            update_window_caption(&new_node->w,!(new_node->w.status&WINDOW_STATUS_INACTIVE));
            //change.
            new_node->node.next = node->node.next;
            new_node->node.prev = node->node.prev;
            if(node->node.prev!=nullptr) {
                node->node.prev->next = &new_node->node;
            }
            if(node->node.next!=nullptr) {
                node->node.next->prev = &new_node->node;
            }
            if(g_window_linked_list.tail==&node->node) {
                g_window_linked_list.tail = &new_node->node;
            }
            //window data copy.
            for(int i=WINDOW_CAPTION_HEIGHT;i<copy_height+WINDOW_CAPTION_HEIGHT;i++) {
                memcpy(((void*)(new_node->w.buffer.value)+width*i*sizeof(U32)),
                       ((void*)(node->w.buffer.value)+orig_width*i*sizeof(U32)),
                       copy_width*sizeof(U32));
                if(copy_width<width) {
                    memset((void*)(new_node->w.buffer.value)+(width*i+copy_width)*sizeof(U32),
                           0xFF,(width-copy_width)*sizeof(U32));
                }
            }
            if(copy_height<height) {
                memset((void*)(new_node->w.buffer.value)+(width*(copy_height+WINDOW_CAPTION_HEIGHT))*sizeof(U32),
                       0xFF,(height-copy_height)*width*sizeof(U32));
            }
            //delete orig node.
            memfree(node);
            return;
        }
    }
}
void update_window_caption(Window* w,U8 activity)
{
    U32* pixeldata = (U32*)w->buffer.value;
    for(int i=0;i<WINDOW_CAPTION_HEIGHT;i++) {
        for(int j=0;j<w->width;j++) {
            pixeldata[i*w->width+j] = activity?WINDOW_CAPTION_COLOR_ACTIVE:WINDOW_CAPTION_COLOR_INACTIVE;
        }
    }
    const U32* close_button   = activity?g_close_button_active:g_close_button_inactive;
    const U32* maximum_button = activity?g_maximize_button_active:g_maximize_button_inactive;
    const U32* minimum_button = activity?g_minimize_button_active:g_minimize_button_inactive;
    int button_count = 0;
    I32 begin_offset = w->width - 10;
    if(!(w->mode&WINDOW_MODE_NO_CLOSE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   close_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
        w->button[button_count++] = CLOSE_BUTTON;
        begin_offset -= 5;
    }
    if(!(w->mode&WINDOW_MODE_NO_MAXIMIZE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   maximum_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
        w->button[button_count++] = MAXIMIZE_BUTTON;
        begin_offset -= 5;
    }
    if(!(w->mode&WINDOW_MODE_NO_MINIMIZE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   minimum_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
        w->button[button_count++] = MINIMIZE_BUTTON;
    }
    int max_caption_offset = begin_offset;
    int caplen = strlen(w->caption);
    begin_offset = (w->width-caplen*WINDOW_TITLE_GLYPH_WIDTH)/2;
    if(begin_offset<0) {
        begin_offset = 0;
    }
    U32 glyph_buffer[WINDOW_TITLE_GLYPH_HEIGHT*WINDOW_CAPTION_BUTTON_WIDTH];
    for(int i=0;i<caplen;i++) {
        render_glyph_font(glyph_buffer,
                        w->caption[i],
                        0xFFFFFFFF,
                        activity?WINDOW_CAPTION_COLOR_ACTIVE:WINDOW_CAPTION_COLOR_INACTIVE);
        for(int j=0;j<WINDOW_TITLE_GLYPH_HEIGHT;j++) {
            memcpy(pixeldata+(4+j)*w->width+begin_offset,
                   glyph_buffer+j*WINDOW_TITLE_GLYPH_WIDTH,
                   WINDOW_TITLE_GLYPH_WIDTH*sizeof(U32));
        }
        begin_offset += WINDOW_TITLE_GLYPH_WIDTH;
        if((begin_offset+WINDOW_TITLE_GLYPH_WIDTH)>max_caption_offset) {
            break;
        }
    }
}
void composite(void)
{
    U32* screen_buffer = g_graphics_data.frame_buffer_base;
    g_screen_lock = 1;
    WindowInlineLinkedListNode* node = (WindowInlineLinkedListNode*)g_window_linked_list.head.next;
    while(node!=nullptr) {
        Window* w = &node->w;
        if(w->status!=WINDOW_STATUS_HIDDEN) {
            U32* window_buffer = (U32*)w->buffer.value;
            int composite_area_left   = (w->x<0)?(-w->x):0;
            int composite_area_right  = ((w->x+w->width)>g_graphics_data.pixels_per_line)?
                                        (g_graphics_data.pixels_per_line-w->x):
                                        w->width;
            int composite_area_top    = (w->y<0)?(-w->y):0;
            int composite_area_bottom = ((w->y+w->height+WINDOW_CAPTION_HEIGHT)>g_graphics_data.pixels_vertical)?
                                        (g_graphics_data.pixels_vertical-w->y):
                                        w->height+WINDOW_CAPTION_HEIGHT;
            int screen_offset_x = w->x>0?w->x:0;
            int screen_offset_y = w->y>0?w->y:0;
            for(int i=composite_area_top;i<composite_area_bottom;i++) {
                memcpy(screen_buffer+(screen_offset_y+(i-composite_area_top))*g_graphics_data.pixels_per_line+screen_offset_x,
                    window_buffer+i*w->width+composite_area_left,
                    (composite_area_right-composite_area_left)*sizeof(U32));
            }
        }
        node = (WindowInlineLinkedListNode*)node->node.next;
    }
    g_screen_lock = 0;
    g_screen_dirty = 1;
}
void send_drag_event(int prev_x,int prev_y,int delta_x,int delta_y)
{
     WindowInlineLinkedListNode* node = (WindowInlineLinkedListNode*)g_window_linked_list.tail;
    int found = 0;
    int changed = 0;
    while(node != (WindowInlineLinkedListNode*)g_window_linked_list.head.next->prev) {
        if(found) {
            if(!(node->w.status & WINDOW_STATUS_INACTIVE)) {
                node->w.status |= WINDOW_STATUS_INACTIVE;
                update_window_caption(&node->w,0);
                changed = 1;
            }
        }
        else {
            if(prev_x>=node->w.x && prev_x<(node->w.x+node->w.width)&&prev_y>=node->w.y && prev_y<(node->w.y+WINDOW_CAPTION_HEIGHT)) {
                found = 1;
                if(node->w.status & WINDOW_STATUS_INACTIVE) {
                    update_window_caption(&node->w,1);
                    if(node!=(WindowInlineLinkedListNode*)g_window_linked_list.tail) {
                        //Send it to the last!
                        WindowInlineLinkedListNode* current_node = node;
                        node->node.next->prev = node->node.prev;
                        node->node.prev->next = node->node.next;
                        insert_existing_inline_node(&g_window_linked_list,(InlineLinkedListNode*)current_node,-1);
                    }
                    node->w.status &= ~(WINDOW_STATUS_INACTIVE);
                }
                //Fill in dirty region.
                graphics_fill_rect(node->w.x,node->w.y,node->w.width,node->w.height+WINDOW_CAPTION_HEIGHT,DEFAULT_BACKGROUND_COLOR);
                node->w.x += delta_x;
                node->w.y += delta_y;
                changed  = 1;
            }
            else {
                if(!(node->w.status & WINDOW_STATUS_INACTIVE)) {
                    node->w.status |= WINDOW_STATUS_INACTIVE;
                    update_window_caption(&node->w,0);
                    changed = 1;
                }
            }
        }
        node = (WindowInlineLinkedListNode*)node->node.prev;
    }
    if(changed) {
        composite();
    }
}
int check_caption_click(Window* w,int x) {
    int offset_from_right = w->width-(x-w->x);
    CaptionButton clicked_button=NO_BUTTON;
    if(offset_from_right<10) {
        return 0;
    }
    if(offset_from_right<10+WINDOW_CAPTION_BUTTON_WIDTH) {
        clicked_button = w->button[0];
        goto check_button;
    }
    if(offset_from_right<10+WINDOW_CAPTION_BUTTON_WIDTH*2+5) {
        clicked_button = w->button[1];
        goto check_button;
    }
    if(offset_from_right<10+WINDOW_CAPTION_BUTTON_WIDTH*3+5*2) {
        clicked_button = w->button[2];
        goto check_button;
    }
check_button:
    switch(clicked_button) {
        case NO_BUTTON: {
            return 0;
        }
        case CLOSE_BUTTON: {
            return 0;
        }
        case MAXIMIZE_BUTTON: {
            if(w->status & WINDOW_STATUS_MAXIMIZED) {
                w->status &= (~WINDOW_STATUS_MAXIMIZED);
                w->x      = w->orig_x;
                w->y      = w->orig_y;
                resize_window(w,w->orig_width,w->orig_height);
                graphics_clear_screen(DEFAULT_BACKGROUND_COLOR);
            }
            else {
                w->orig_height = w->height;
                w->orig_width  = w->width;
                w->orig_x      = w->x;
                w->orig_y      = w->y;
                w->x           = 0;
                w->y           = 0;
                w->status |= WINDOW_STATUS_MAXIMIZED;
                resize_window(w,g_graphics_data.pixels_per_line,
                              g_graphics_data.pixels_vertical-WINDOW_CAPTION_HEIGHT);
            }
            if(w->code_on_resize!=nullptr) {
                w->ui_thread->stack[++w->ui_thread->rsp].data = (U64)w->event_receiver;//this
                w->ui_thread->stack[w->ui_thread->rsp].type   = STACK_TYPE_REFERENCE;
                w->ui_thread->stack[++w->ui_thread->rsp].data = (U64)w->width;
                w->ui_thread->stack[w->ui_thread->rsp].type   = STACK_TYPE_INT;
                w->ui_thread->stack[++w->ui_thread->rsp].data = (U64)w->height;
                w->ui_thread->stack[w->ui_thread->rsp].type   = STACK_TYPE_INT;
                invoke_method(w->ui_thread,
                              w->event_receiver->parent_class,
                              w->code_on_resize,
                              3);
            }
            return 1;
        }
        case MINIMIZE_BUTTON: {
            return 0;
        }
    }
    return 0;
}
void send_click_event(int x,int y)
{
    WindowInlineLinkedListNode* node = (WindowInlineLinkedListNode*)g_window_linked_list.tail;
    int found = 0;
    int changed = 0;
    while(node != (WindowInlineLinkedListNode*)g_window_linked_list.head.next->prev) {
        if(found) {
            if(!(node->w.status & WINDOW_STATUS_INACTIVE)) {
                node->w.status |= WINDOW_STATUS_INACTIVE;
                update_window_caption(&node->w,0);
                changed = 1;
            }
        }
        else {
            if(x>=node->w.x && x<(node->w.x+node->w.width)&&y>=node->w.y && y<(node->w.y+WINDOW_CAPTION_HEIGHT+node->w.height)) {
                found = 1;
                if(node->w.status & WINDOW_STATUS_INACTIVE) {
                    update_window_caption(&node->w,1);
                    if(node!=(WindowInlineLinkedListNode*)g_window_linked_list.tail) {
                        //Send it to the last!
                        WindowInlineLinkedListNode* current_node = node;
                        node->node.next->prev = node->node.prev;
                        node->node.prev->next = node->node.next;
                        insert_existing_inline_node(&g_window_linked_list,(InlineLinkedListNode*)current_node,-1);
                    }
                    changed  = 1;
                    node->w.status &= ~(WINDOW_STATUS_INACTIVE);
                }
                if(y<(node->w.y+WINDOW_CAPTION_HEIGHT)) {
                    changed |= check_caption_click(&node->w,x);
                    goto click_out;
                }
                if(node->w.code_on_click!=nullptr) {
                    node->w.ui_thread->stack[++node->w.ui_thread->rsp].data = (U64)node->w.event_receiver;//this
                    node->w.ui_thread->stack[node->w.ui_thread->rsp].type   = STACK_TYPE_REFERENCE;
                    node->w.ui_thread->stack[++node->w.ui_thread->rsp].data = x-(U64)node->w.x;
                    node->w.ui_thread->stack[node->w.ui_thread->rsp].type   = STACK_TYPE_INT;
                    node->w.ui_thread->stack[++node->w.ui_thread->rsp].data = y-(U64)node->w.y-WINDOW_CAPTION_HEIGHT;
                    node->w.ui_thread->stack[node->w.ui_thread->rsp].type   = STACK_TYPE_INT;
                    invoke_method(node->w.ui_thread,
                                  node->w.event_receiver->parent_class,
                                  node->w.code_on_click,
                                  3);
                    }
            }
            else {
                if(!(node->w.status & WINDOW_STATUS_INACTIVE)) {
                    node->w.status |= WINDOW_STATUS_INACTIVE;
                    update_window_caption(&node->w,0);
                    changed = 1;
                }
            }
        }
        node = (WindowInlineLinkedListNode*)node->node.prev;
    }
click_out:
    if(changed) {
        composite();
    }

}
static int g_prev_clicked=0;
static int g_drag_begin_x,g_drag_begin_y;
void window_mouse_event_receiver(int x,int y,int left,int right,int mid)
{
    if(left) {
        if(!g_prev_clicked) {
            g_prev_clicked = 1;
            g_drag_begin_x = x;
            g_drag_begin_y = y;
        }
    }
    else {
        if(g_prev_clicked) {
            if(x-g_drag_begin_x==0&&y-g_drag_begin_y==0) {
                //click.
                send_click_event(x,y);
            }
            else {
                send_drag_event(g_drag_begin_x,g_drag_begin_y,x-g_drag_begin_x,y-g_drag_begin_y);
            }
            g_prev_clicked = 0;
        }
    }
}
