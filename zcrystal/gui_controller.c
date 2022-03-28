#include <zcrystal/gui_controller.h>
#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>

U8 g_aero_enable;
U8 g_mouse_is_render;

MousePosition g_mouse_info;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable)
{
    gui_init_window_manager(gui_aero_enable);
    g_aero_enable = gui_aero_enable;
    // set mouse to center
    g_mouse_info.horizontal = g_screen_resolution.horizontal * 0.5;
    g_mouse_info.vertical = g_screen_resolution.vertical * 0.5;
    g_mouse_info.status = 0;
    gui_trigger_screen_update();
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
    WindowManageData* temp_pointer;
    temp_pointer = g_window_list_bottom;
    while (temp_pointer->prev != NULL)
    {
        gui_render_window(*temp_pointer);
        temp_pointer = temp_pointer->prev;
    }
    gui_render_mouse(g_mouse_info.horizontal, g_mouse_info.vertical, g_mouse_info.status);
    return 1;
}

U8 gui_trigger_cpu_mouse_render_disable(U8 value)
{
    if (value == 1)
    {
        g_mouse_is_render = 1;
    }
    else if (value == 0)
    {
        g_mouse_is_render = 0;
    }
    else
    {
        return 0;
    }
    return 1;
}

