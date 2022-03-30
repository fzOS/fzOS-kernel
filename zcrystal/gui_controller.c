#include <zcrystal/gui_controller.h>
#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>
#include <drivers/fbcon.h>
#include <common/printk.h>

#define _GUI_VERSION_H "0.1.0"

U8 g_aero_enable;
U8 g_mouse_is_render;
U16 g_window_bar_height;

MousePosition g_mouse_info;
MouseLocIndent g_mouse_loc;

// set up layer index for mouse click & render
U8 gui_init_main_controller(int gui_aero_enable)
{
    g_window_bar_height = _GUI_DEFAULT_1080P_WINDOW_BAR_HEIGHT_;
    gui_init_window_manager(gui_aero_enable);
    printk("---GUI Initiated---\n");
    printk("Screen Resolution: %dx%d\n", g_screen_resolution.horizontal, g_screen_resolution.vertical);
    printk("Zcrystal GUI Version %s\n", _GUI_VERSION_H);
    g_aero_enable = gui_aero_enable;
    g_mouse_info.horizontal = 0;
    g_mouse_info.vertical = 0;
    g_mouse_info.status = _GUI_MOUSE_RELEASED_;
    // set mouse to center
    MousePosition mouse_info_temp;
    mouse_info_temp.horizontal = g_screen_resolution.horizontal * 0.5;
    mouse_info_temp.vertical = g_screen_resolution.vertical * 0.5;
    mouse_info_temp.status = _GUI_MOUSE_RELEASED_;
    U16 window_uid;
    g_mouse_loc.position_type = gui_locate_mouse_position(mouse_info_temp.horizontal, mouse_info_temp.vertical, &window_uid);
    g_mouse_loc.window_uid = window_uid;
    gui_set_mouse_status(mouse_info_temp);
    gui_trigger_screen_update();
    return 1;
}

U8 gui_set_mouse_status(MousePosition input_mouse)
{
    if (g_mouse_info.horizontal != input_mouse.horizontal
        || g_mouse_info.vertical != input_mouse.vertical)
    {
        if (g_mouse_info.status == _GUI_MOUSE_RELEASED_ || (g_mouse_loc.position_type != _GUI_WINDOW_POS_BAR_ && g_mouse_loc.position_type != _GUI_WINDOW_POS_INNER_))
        {
            // if the mouse is not pressed, or is pressed but on control button, update the current position of mouse
            U16 window_uid;
            g_mouse_loc.position_type = gui_locate_mouse_position(input_mouse.horizontal, input_mouse.vertical, &window_uid);
            g_mouse_loc.window_uid = window_uid;
        }
        
        // if the status change, consider need change window focus and move
        if (g_mouse_info.status == input_mouse.status)
        {
            // check if on bar and is on action of moving the window
            if (g_mouse_info.status == _GUI_MOUSE_PRESSED_ && g_mouse_loc.position_type == _GUI_WINDOW_POS_BAR_)
            {
                // moving the window position
                WindowManageData* temp_pointer = gui_window_manager_get_window_pointer(g_mouse_loc.window_uid);
                temp_pointer->start_point_h += input_mouse.horizontal - g_mouse_info.horizontal;
                temp_pointer->start_point_v += input_mouse.vertical - g_mouse_info.vertical;
                // add to here if need special update function
            }
        }

        // update the mouse as final action, which can keep the position last time
        g_mouse_info.horizontal = input_mouse.horizontal;
        g_mouse_info.vertical = input_mouse.vertical;
    }

    // check the action of mouse;
    if (g_mouse_info.status == _GUI_MOUSE_RELEASED_ && input_mouse.status == _GUI_MOUSE_PRESSED_)
    {
        printk("Onclick\n");
        printk("top window uid: %d \n", g_window_list_top->UID);
        printk("target window uid: %d \n", g_mouse_loc.window_uid);
        // pressing, check the location and focus
        if (g_mouse_loc.window_uid != g_window_list_top->UID)
        {
            // change focus
            gui_window_manager_focus_change(g_mouse_loc.window_uid);
        }
    }
    else
    {
        // on release
            switch (g_mouse_loc.position_type)
            {
            case _GUI_WINDOW_POS_CLOSE_:
                gui_window_manager_destroy_window(g_window_list_top->PID ,g_mouse_loc.window_uid);
                break;
            case _GUI_WINDOW_POS_MIN_:
                g_window_list_top->is_hide = 1;
                break;
            default:
                break;
            }
    }
    g_mouse_info.status = input_mouse.status;
    /*
    printk("Current Mouse Loc: %d\n", g_mouse_loc.position_type);
    switch (g_mouse_info.status)
    {
    case _GUI_MOUSE_PRESSED_:
        printk("Current Mouse Left: Pressed\n");
        break;
    case _GUI_MOUSE_RELEASED_:
        printk("Current Mouse Left: Released\n");
        break;
    default:
        printk("Current Mouse Left: %d\n", g_mouse_info.status);
        break;
    }
    */
    return 1;
}

U8 gui_locate_mouse_position(U16 mouse_h, U16 mouse_v, U16* window_uid)
{
    WindowManageData* temp_pointer;
    // initial the position to bottom(the desktop layer)
    U8 position_ident = _GUI_WINDOW_POS_INNER_;
    *window_uid = g_window_list_bottom->UID;
    temp_pointer = g_window_list_top;
    while (temp_pointer != NULL)
    {
        // check if is in current window
        if (mouse_h >= temp_pointer->start_point_h && mouse_h <= (temp_pointer->start_point_h + temp_pointer->base_info.horizontal)
            && mouse_v >= temp_pointer->start_point_v && mouse_v <= (temp_pointer->start_point_v + temp_pointer->base_info.vertical))
        {
            // in the current windows
            // set the window uid
            *window_uid = temp_pointer->UID;
            // check the window position
            if (mouse_v <= (temp_pointer->start_point_v + g_window_bar_height) && temp_pointer->window_bar_mode != WINDOW_MODE_BORDERLESS)
            {
                // in the bar area
                position_ident = _GUI_WINDOW_POS_BAR_;
                U16 temp_top_bar_width = temp_pointer->start_point_h + temp_pointer->base_info.horizontal;
                U16 temp_top_control_width = g_window_bar_height * 4 / 3;
                for (U8 i = 0; i < 3; i++)
                {
                    // check the control button
                    if (mouse_h < temp_top_bar_width && mouse_h > (temp_top_bar_width - temp_top_control_width))
                    {
                        // get the control point
                        U32* temp_pointer_color;
                        temp_pointer_color = temp_pointer->base_info.frame_buffer_base;
                        U32 point_color = *(temp_pointer_color + (mouse_h-temp_pointer->start_point_h) + (mouse_v-temp_pointer->start_point_v)*temp_pointer->base_info.horizontal);
                        if ( point_color != _GUI_DEFAULT_APPLICATION_BAR_COLOR_ || point_color != _GUI_DEFAULT_APPLICATION_BAR_COLOR_INACTIVE_)
                        {
                            switch (i)
                            {
                            case 0:
                                if (temp_pointer->window_bar_mode != WINDOW_MODE_NO_CLOSE)
                                {
                                    position_ident = _GUI_WINDOW_POS_CLOSE_;
                                }
                                else
                                {
                                    position_ident = _GUI_WINDOW_POS_MIN_;
                                }
                                break;
                            case 1:
                                if (temp_pointer->window_bar_mode == WINDOW_MODE_NORMAL)
                                {
                                    position_ident = _GUI_WINDOW_POS_MIN_;
                                }
                                else
                                {
                                    position_ident = _GUI_WINDOW_POS_MAX_;
                                }
                                break;
                            case 2:
                                position_ident = _GUI_WINDOW_POS_MAX_;
                                break;
                            }
                        }
                        break;
                    }
                    temp_top_bar_width -= temp_top_control_width;
                }
            }
            break;
        }
        temp_pointer = temp_pointer->next;
    }
    return position_ident;
}

// update whole screen
U8 gui_trigger_screen_update()
{
    WindowManageData* temp_pointer;
    temp_pointer = g_window_list_bottom;
    while (temp_pointer != NULL)
    {
        gui_render_window(*temp_pointer);
        temp_pointer = temp_pointer->prev;
    }
    // gui_render_mouse(g_mouse_info.horizontal, g_mouse_info.vertical, g_mouse_info.status);
    return 1;
}

U8 gui_trigger_cpu_mouse_render_disable(U8 value)
{
    if (value == 1)
    {
        g_mouse_is_render = 0;
    }
    else if (value == 0)
    {
        g_mouse_is_render = 1;
    }
    else
    {
        return 0;
    }
    return 1;
}

