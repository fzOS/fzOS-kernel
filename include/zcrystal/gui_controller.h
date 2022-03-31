#ifndef _GUI_MAIN_CONTROLLER_H
#define _GUI_MAIN_CONTROLLER_H
#include <types.h>

#define _GUI_WINDOW_POS_BAR_      0
#define _GUI_WINDOW_POS_INNER_    1
#define _GUI_WINDOW_POS_CLOSE_    2
#define _GUI_WINDOW_POS_MIN_      3
#define _GUI_WINDOW_POS_MAX_      4

#define _GUI_MOUSE_RELEASED_      0
#define _GUI_MOUSE_PRESSED_       255

#define _GUI_MOUSE_ONPRESS_ACTION_    1
#define _GUI_MOUSE_ONRELEASE_ACTION_  2

#define _GUI_DEFAULT_1080P_WINDOW_BAR_HEIGHT_ 30

#define _GUI_WINDOW_VERTICAL_MIN_   0
#define _GUI_WINDOW_HORIZONTAL_MIN_   0

#define _GUI_WINDOW_VERTICAL_MAX_ADD_   200
#define _GUI_WINDOW_HORIZONTAL_MAX_ADD_   200

typedef struct {
    U16 horizontal;
    U16 vertical;
    // 0: release; 1: pressed; XD no scroll now
    U8 status;    
} MousePosition;

typedef struct {
    U16 window_uid;
    U8 position_type;
} MouseLocIndent;


extern U8 g_aero_enable;
extern U8 g_mouse_is_render;
extern U16 g_window_bar_height;

extern volatile U8 g_gui_render_lock;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable);
U8 gui_set_mouse_status(MousePosition input_mouse);
// return the type including GUI_WINDOW_POS_XXX
U8 gui_locate_mouse_position(U16 mouse_h, U16 mouse_v, U16* window_uid);
// update whole screen
U8 gui_trigger_screen_update();
U8 gui_trigger_cpu_mouse_render_disable(U8 value);

#endif
