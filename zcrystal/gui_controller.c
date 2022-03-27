#include <zcrystal/gui_controller.h>
#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>

U8 g_aero_enable;

MousePosition g_mouse_info;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable)
{
    gui_init_window_manager(gui_aero_enable);
    aero_enable = gui_aero_enable;
    // set mouse to center
    g_mouse_info.horizontal = g_screen_resolution.horizontal * 0.5;
    g_mouse_info.vertical = g_screen_resolution.vertical * 0.5;
    g_mouse_info.status = 0;
    return 1;
}

U8 gui_set_mouse_status(MousePosition input_mouse)
{
    g_mouse_info.horizontal = input_mouse.horizontal;
    g_mouse_info.vertical = input_mouse.vertical;
    g_mouse_info.status = input_mouse.status;
    return 1;
}

// update whole screen
U8 gui_trigger_screen_update()
{

}

