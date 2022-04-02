#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER
#include <zcrystal/window.h>
extern InlineLinkedList g_window_linked_list;
extern volatile int g_screen_lock;
FzOSResult enter_graphical_mode(void);
FzOSResult exit_graphical_mode(void);
void update_window_caption(Window* w,U8 activity);
Window* create_window(U32 x,U32 y,U32 width,U32 height,char* title,object* event_receiver);
void composite(void);
void window_mouse_event_receiver(int x,int y,int left,int right,int mid);
#endif
