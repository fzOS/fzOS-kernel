#include <zcrystal/gui_controller.h>
#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>
#include <drivers/graphics.h>

U8 gui_render_draw_application_bar(WindowData* window)
{
    U32* temp_frame_buffer_pt;
    temp_frame_buffer_pt = window->frame_buffer_base;
    for (U16 i = 0; i < 30; i++)
    {
        for (U16 j = 0; j < window->horizontal; j++)
        {
            *temp_frame_buffer_pt = _GUI_DEFAULT_APPLICATION_BAR_COLOR_;
            // move pointer to next
            temp_frame_buffer_pt += 1;
        }
    }
    return 1;
}

U8 gui_render_preset_window(WindowData* window, U8 if_loading)
{
    // this function is for clear the created window
    U32* temp_frame_buffer_pt;
    temp_frame_buffer_pt = window->frame_buffer_base;
    for (U16 i = 0; i < window->vertical; i++)
    {
        for (U16 j = 0; j < window->horizontal; j++)
        {
            if (if_loading)
            {
                *temp_frame_buffer_pt = _GUI_DEFAULT_DESKTOP_BACKGOUND_COLOR_;
            }
            else
            {
                *temp_frame_buffer_pt = _GUI_DEFAULT_BACKGOUND_COLOR_;
            }
            // move pointer to next
            temp_frame_buffer_pt += 1;
        }
    }
    if (!if_loading)
    {
        gui_render_draw_application_bar(window);
    }
    return 1;
}

U8 gui_render_window(WindowManageData layer_to_draw)
{
    U8 draw_signal = 1;
    // these are avoid write out side gop buffer
    U16 max_horizontal;
    U16 max_vertical;
    if (layer_to_draw.start_point_h + layer_to_draw.base_info.horizontal > g_screen_resolution.horizontal)
    {
        max_horizontal = g_screen_resolution.horizontal;
        if (layer_to_draw.start_point_h > g_screen_resolution.horizontal)
        {
            draw_signal = 0;
            return 0;
        }
    }
    else
    {
        max_horizontal = layer_to_draw.start_point_h + layer_to_draw.base_info.horizontal;
    }
    
    if (layer_to_draw.start_point_v + layer_to_draw.base_info.vertical > g_screen_resolution.vertical)
    {
        max_vertical = g_screen_resolution.vertical;
        if (layer_to_draw.start_point_v > g_screen_resolution.vertical)
        {
            draw_signal = 0;
            return 0;
        }
    }
    else
    {
        max_vertical = layer_to_draw.start_point_v + layer_to_draw.base_info.vertical;
    }

    U32* temp_frame_buffer_pt;
    temp_frame_buffer_pt = layer_to_draw.base_info.frame_buffer_base;
    // start draw this to gop buffer here
    if (draw_signal)
    {
        for (U16 i = layer_to_draw.start_point_v; i < max_vertical; i++)
        {
            for (U16 j = layer_to_draw.start_point_h; j < max_horizontal; j++)
            {
                graphics_draw_pixel(j, i, *(temp_frame_buffer_pt + j - layer_to_draw.start_point_h));
            }
            temp_frame_buffer_pt += layer_to_draw.base_info.horizontal;
        }
    }
    return 1;
}

U8 gui_render_mouse(U16 mouse_pos_h, U16 mouse_pos_v, U8 status)
{
    if (g_mouse_is_render)
    {
        if (mouse_pos_h < g_screen_resolution.horizontal && (mouse_pos_h + 10) < g_screen_resolution.horizontal
            && mouse_pos_v < g_screen_resolution.vertical && (mouse_pos_v + 10) < g_screen_resolution.vertical)
        {
            for (int i = mouse_pos_v; i < (mouse_pos_v + 10); i++)
            {
                for (int j = mouse_pos_h; j < (mouse_pos_v + 10); j++)
                {
                    graphics_draw_pixel(j, i, 0x00000000);
                }
            }
        }
    }
    return 0;
}
