
#ifndef _GUI_WINDOW_MANAGER_H
#define _GUI_WINDOW_MANAGER_H
#include <types.h>
#include <drivers/graphics.h>
#include <drivers/fbcon.h>
/*
#include <uefivars.h>
#include <types.h>
typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  U32 *frame_buffer_base;
  U32 pixels_per_line;
  U32 default_background_color;
} GraphicsData; 
*/
typedef struct {
  U16 horizontal;
  U16 vertical;
} ScreenDefinition; 

typedef struct {
    /* data */
    U16 horizontal;
    U16 vertical;
    U32 *frame_buffer_base;
} WindowData;

typedef struct
{
    WindowData base_info;
    U16 PID;
    U16 start_point_h;
    U16 start_point_v;
    U8 in_use;
    U8 is_hide;
} WindowManageData;

typedef struct
{
    U16 window_index;
    U8  window_in_use;
} WindowLayerConfig;

typedef struct{
    U16 window_in_use;
    U16 window_available;
    U16 window_focus_index;
    U8 window_in_loading;
    U16 max_window_layer_in_use;
    WindowLayerConfig *layer_index;
} WindowManageConfig;

U8 gui_init_window_manager(int gui_aero_enable);
U8 gui_window_manager_offline();
U8 gui_loading_screen_request(WindowData *info_receiver);
U8 gui_trigger_loading_screen_status(U8 status);
// following functions available
// focus mode: 0 for focus at top layer, 1 for trigger lower than it
U8 gui_window_manager_create_window(U16 PID, U8 focus_mode, WindowData *info_receiver);
// if want later allow the c program/or change this to system call
U8 gui_window_manager_get_window_info(U16 PID, U16 window_index, WindowData *info_receiver);
U8 gui_window_manager_focus_change(U16 window_index);
U8 gui_window_manager_destroy_window(U16 PID, U16 window_index);
void gui_log_print_hand_over(CharDev* dev, U8 c);
void gui_log_flush_hand_over(CharDev* dev);