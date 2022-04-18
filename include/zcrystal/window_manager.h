#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER
#include <zcrystal/window.h>
extern InlineLinkedList g_window_linked_list;
extern volatile int g_screen_lock;
FzOSResult enter_graphical_mode(void);
FzOSResult exit_graphical_mode(void);
void update_window_caption(Window* w,U8 activity);
Window* create_window(U32 x,U32 y,U32 width,U32 height,char* title,int attr, object* event_receiver,thread* ui_thread);
void composite(void);
void register_window_callbacks(Window* w);
void set_sprite(Window* w,void* data,int width,int height);
void update_sprite_position(Window* w,int x,int y);
void window_mouse_event_receiver(int x,int y,int left,int right,int mid);
#endif
