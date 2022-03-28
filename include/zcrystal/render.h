#ifndef _GUI_SCREEN_RENDER_API_H
#define _GUI_SCREEN_RENDER_API_H

#define _GUI_DEFAULT_APPLICATION_BAR_COLOR_ 0x00498AAA
#define _GUI_DEFAULT_DESKTOP_BACKGOUND_COLOR_ 0x00B0BEDA
#define _GUI_DEFAULT_BACKGOUND_COLOR_ 0x001E1E1E

#include <zcrystal/window_manager.h>

U8 gui_render_draw_application_bar(WindowData* window);
U8 gui_render_preset_window(WindowData* window);
U8 gui_render_window(WindowManageData layer_to_draw);
U8 gui_render_mouse(U16 mouse_pos_h, U16 mouse_pos_v, U8 status);

#endif
