#ifndef _GUI_MAIN_CONTROLLER_H
#define _GUI_MAIN_CONTROLLER_H
#include <types.h>

typedef struct {
    U16 horizontal;
    U16 vertical;
    // 0: release; 1: pressed; XD no scroll now
    U8 status;    
} MousePosition;

extern U8 aero_enable;
extern U8 g_mouse_is_render;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable);
U8 gui_set_mouse_status(MousePosition input_mouse);
// update whole screen
U8 gui_trigger_screen_update();
U8 gui_trigger_cpu_mouse_render_disable(U8 value);

#endif