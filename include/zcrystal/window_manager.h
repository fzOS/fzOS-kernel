#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER
#include <zcrystal/window.h>
extern InlineLinkedList g_window_linked_list;
FzOSResult enter_graphical_mode(void);
FzOSResult exit_graphical_mode(void);
void update_window_caption(Window* w,U8 activity);
Window* create_window(U32 x,U32 y,U32 width,U32 height,char* title,object* event_receiver);
void composite(void);
#endif
