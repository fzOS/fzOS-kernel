#include <zcrystal/window_manager.h>
#include <zcrystal/window_console.h>
#include <zcrystal/window_decoration.h>
#include <drivers/asciifont.h>
#include <drivers/fbcon.h>
#include <memory/memory.h>
#include <drivers/vmsvga.h>
#include <common/kstring.h>
static U64 g_next_window_id;
volatile int g_screen_lock = 0;
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
FzOSResult enter_graphical_mode(void)
{
    //First,we disable console.
    hook_window_console();
    //Then,we clear the whole screen.
    graphics_clear_screen(DEFAULT_BACKGROUND_COLOR);
    //After that, We create the debug window.
    create_window(100,100,600,400,"Debug Log",nullptr);
    create_window(200,200,800,600,"Test Window #2",nullptr);
    create_window(400,400,300,500,"Test Window #3",nullptr);
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
    const U32* close_button   = activity?g_close_button_active:g_close_button_inactive;
    const U32* maximum_button = activity?g_maximize_button_active:g_maximize_button_inactive;
    const U32* minimum_button = activity?g_minimize_button_active:g_minimize_button_inactive;
    I32 begin_offset = w->width - 10;
    if(!(w->mode&WINDOW_MODE_NO_CLOSE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   close_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
        begin_offset -= 5;
    }
    if(!(w->mode&WINDOW_MODE_NO_MAXIMIZE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   maximum_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
        begin_offset -= 5;
    }
    if(!(w->mode&WINDOW_MODE_NO_MINIMIZE)) {
        begin_offset -= WINDOW_CAPTION_BUTTON_WIDTH;
        for(int i=0;i<WINDOW_CAPTION_BUTTON_HEIGHT;i++) {
            memcpy(pixeldata+(4+i)*w->width+begin_offset,
                   minimum_button+i*WINDOW_CAPTION_BUTTON_WIDTH,
                   WINDOW_CAPTION_BUTTON_WIDTH*sizeof(U32));
        }
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
        //TODO:check visibility.
        Window* w = &node->w;
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
        for(int i=composite_area_top;i<composite_area_bottom;i++) {
            memcpy(screen_buffer+(screen_offset_y+i)*g_graphics_data.pixels_per_line+screen_offset_x,
                   window_buffer+i*w->width+composite_area_left,
                   (composite_area_right-composite_area_left)*sizeof(U32));
        }
        node = (WindowInlineLinkedListNode*)node->node.next;
    }
    g_screen_lock = 0;
    g_screen_dirty = 1;
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
static int g_is_dragging=0;
static int g_prev_x,g_prev_y;
void window_mouse_event_receiver(int x,int y,int left,int right,int mid)
{
    if(left) { //clicked.
        g_is_dragging = 1;
        g_prev_x = x;
        g_prev_y = y;
    }
    else {
        if(g_is_dragging) {
            int delta_x = x - g_prev_x;
            int delta_y = y - g_prev_y;
            //TODO:Draging event.
            if(delta_x==0&&delta_y==0) {
                //Act like a Click.
                send_click_event(x,y);
            }
        }
        g_is_dragging = 0;
    }
}
