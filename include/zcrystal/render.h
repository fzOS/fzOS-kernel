#ifndef _GUI_SCREEN_RENDER_API_H
#define _GUI_SCREEN_RENDER_API_H

#define _GUI_DEFAULT_APPLICATION_BAR_COLOR_ 0x00498AAA
#define _GUI_DEFAULT_APPLICATION_BAR_COLOR_INACTIVE_ 0x00747D8F
#define _GUI_DEFAULT_DESKTOP_BACKGOUND_COLOR_ 0x00B0BEDA
#define _GUI_DEFAULT_BACKGOUND_COLOR_ 0x001E1E1E

#define _GUI_DEFAULT_APP_BAR_CLOSE_COLOR_ 0x00FF5F57
#define _GUI_DEFAULT_APP_BAR_MAX_COLOR_ 0x0028C840
#define _GUI_DEFAULT_APP_BAR_MIN_COLOR_ 0x00FEBC2E

#include <zcrystal/window_manager.h>

U8 gui_render_application_bar_status_change(WindowData* window, U8 status);
U8 gui_render_draw_application_bar(WindowData* window, U8 preset);
U8 gui_render_preset_window(WindowData* window, U8 if_loading, U8 preset);
U8 gui_render_window(WindowManageData layer_to_draw);
U8 gui_render_mouse(U16 mouse_pos_h, U16 mouse_pos_v, U8 status);
void gui_draw_cycle(U32* pt, U16 horz_resol, U16 hc, U16 vc, U16 r, U32 color);
void gui_cycle_fill_8(U32* pt, U16 horz_resol, U16 hc, U16 vc, I32 x, I32 y, U32 color);
#endif
