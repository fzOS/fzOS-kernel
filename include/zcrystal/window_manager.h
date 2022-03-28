
#ifndef _GUI_WINDOW_MANAGER_H
#define _GUI_WINDOW_MANAGER_H
#include <types.h>
#include <drivers/chardev.h>

#define WINDOW_MODE_NORMAL       0x00000000
#define WINDOW_MODE_NO_MAXIMIZE  0x00000001
#define WINDOW_MODE_NO_MINIMIZE  0x00000002
#define WINDOW_MODE_NO_CLOSE     0x00000004
#define WINDOW_MODE_BORDERLESS   0x00000008
#define WINDOW_STATUS_NORMAL     0x00000000
#define WINDOW_STATUS_HIDDEN     0x00000001
#define WINDOW_STATUS_INACTIVE   0x00000002
#define WINDOW_STATUS_LOADING    0x00000004
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
} WindowDataExport;

typedef struct {
    /* data */
    U16 horizontal;
    U16 vertical;
    U32 *frame_buffer_base;
    U32 *frame_buffer_base_User;
} WindowData;

typedef struct WindowManageData
{
    WindowData base_info;
    U16 PID;
    // uid is for multi window use of same process
    // doubled size for avoid oversize issue
    U32 UID;
    U16 start_point_h;
    U16 start_point_v;
	U8 window_bar_mode;
    U8 is_hide;
    // window name
    struct WindowManageData* prev;
    struct WindowManageData* next;
} WindowManageData;

U8 gui_init_window_manager(int gui_aero_enable);
U8 gui_window_manager_offline();
U8 gui_loading_screen_request(WindowDataExport *info_receiver);
U8 gui_trigger_loading_screen_status(U8 status);
// following functions available
// focus mode: use these defines
// overrided: 1 for focus at top layer, 0 for trigger lower than it, 2 for initial use
U8 gui_window_manager_create_window(U16 PID, U8 focus_status, U8 window_mode, U16 pos_h, U16 pos_v, U16 size_h, U16 size_v, WindowDataExport *info_receiver);
WindowManageData* gui_window_manager_get_window_pointer(U32 unique_id);
// if want later allow the c program/or change this to system call
U8 gui_window_manager_get_window_info(U16 PID, U32 unique_id, WindowDataExport *info_receiver);
U8 gui_window_manager_focus_change(U32 unique_id);
U8 gui_window_manager_destroy_window(U16 PID, U32 unique_id);
void gui_log_print_hand_over(CharDev* dev, U8 c);
void gui_log_flush_hand_over(CharDev* dev);

extern WindowManageData *g_window_list_top;
extern WindowManageData *g_window_list_bottom;
extern ScreenDefinition g_screen_resolution;
#endif
