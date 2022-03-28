#include <zcrystal/gui_controller.h>
#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>
#include <drivers/graphics.h>

// status 0 is change to inactive, 1 is to active
U8 gui_render_application_bar_status_change(WindowData* window, U8 status)
{
    U32* temp_frame_buffer_pt;
    temp_frame_buffer_pt = window->frame_buffer_base;
    if (!status)
    {
        for (U16 i = 0; i < 30; i++)
        {
            for (U16 j = 0; j < window->horizontal; j++)
            {
                if (*temp_frame_buffer_pt == _GUI_DEFAULT_APPLICATION_BAR_COLOR_)
                {
                    *temp_frame_buffer_pt = _GUI_DEFAULT_APPLICATION_BAR_COLOR_INACTIVE_;
                }
                // move pointer to next
                temp_frame_buffer_pt += 1;
            }
        }
    }
    else
    {
        for (U16 i = 0; i < 30; i++)
        {
            for (U16 j = 0; j < window->horizontal; j++)
            {
                if (*temp_frame_buffer_pt == _GUI_DEFAULT_APPLICATION_BAR_COLOR_INACTIVE_)
                {
                    *temp_frame_buffer_pt = _GUI_DEFAULT_APPLICATION_BAR_COLOR_;
                }
                // move pointer to next
                temp_frame_buffer_pt += 1;
            }
        }
    }
    return 1;
}

U8 gui_render_draw_application_bar(WindowData* window, U8 preset)
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
    // close, min, max
    U8 draw[3] = {1, 1, 1};
    switch (preset)
    {
    case WINDOW_MODE_NO_CLOSE:
        draw[0] = 0;
        break;
    case WINDOW_MODE_NO_MAXIMIZE:
        draw[2] = 0;
        break;
    case WINDOW_MODE_NO_MINIMIZE:
        draw[1] = 0;
        break;
    case WINDOW_MODE_NORMAL:
    default:
        // do nothing
        break;
    }
    // 每个格子 30 * 30, 圆半径 10,
    U16 v_center = 15;
    U16 h_center = window->horizontal - 15;
    U16 r = 7;
    U32 temp_color;
    for (U8 i = 0; i < 3; i++)
    {
        if (draw[i])
        {
            switch (i)
            {
            case 0:
                temp_color = _GUI_DEFAULT_APP_BAR_CLOSE_COLOR_;
                break;
            case 1:
                temp_color = _GUI_DEFAULT_APP_BAR_MIN_COLOR_;
                break;
            case 2:
                temp_color = _GUI_DEFAULT_APP_BAR_MAX_COLOR_;
                break;
            }
            gui_draw_cycle(window->frame_buffer_base, window->horizontal, h_center, v_center, r, temp_color);
            h_center -= 20;
        }
    }
    return 1;
}

U8 gui_render_preset_window(WindowData* window, U8 if_loading, U8 preset)
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
        gui_render_draw_application_bar(window, preset);
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

void gui_cycle_fill_8(U32* pt, U16 horz_resol, U16 hc, U16 vc, I32 x, I32 y, U32 color)
{
    U32 a = vc * horz_resol;
    U32 b = y * horz_resol;
    U32 c = x * horz_resol;
    *(pt + a + b + hc+x) = color;
    *(pt + a + b + hc-x) = color;
    *(pt + a - b + hc+x) = color;
    *(pt + a - b + hc-x) = color;
    *(pt + a + c + hc+y) = color;
    *(pt + a + c + hc-y) = color;
    *(pt + a - c + hc+y) = color;
    *(pt + a - c + hc-y) = color;
}

void gui_draw_cycle(U32* pt, U16 horz_resol, U16 hc, U16 vc, U16 r, U32 color)
{
    I32 x, y, yi, d;
    y = (I32)r;
    x = 0;
    d = 1-(I32)r;
    while (x <= y)
    {
        for (yi = x; yi <= y; yi++)
        {
            gui_cycle_fill_8(pt, horz_resol, hc, vc, x, yi, color);
        }
        if (d <= 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * (x-y) + 5;
            y--;
        }
        x++;
    }
}