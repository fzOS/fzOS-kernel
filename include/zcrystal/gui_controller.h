#ifndef _GUI_MAIN_CONTROLLER_H
#define _GUI_MAIN_CONTROLLER_H
#include <types.h>

typedef struct {
    U16 horizontal;
    U16 vertical;
    // 0: release; 1: pressed; XD no scroll now
    U8 status;    
} MousePosition; 

typedef struct {
    /* data */
    U16 horizontal_size;
    U16 vertical_size;
    // also use as mouse click interface
    U16 *window_render_index; 
} ScreenControlIndex;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable);
U8 gui_set_mouse_status(MousePosition input_mouse);
// update whole screen
U8 gui_trigger_screen_update();
