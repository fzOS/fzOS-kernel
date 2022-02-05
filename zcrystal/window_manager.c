#include <zcrystal/window_manager.h>
#include <drivers/graphics.h>
#include <memory/memory.h>
#include <types.h>

ScreenDefinition g_screen_resolution;
// window list
WindowManageData *g_window_list;
// in use and available
WindowManageConfig g_window_config;
U8 g_gui_in_loading_screen = 1;

void gui_log_print_hand_over(CharDev* dev, U8 c)
{
    
}

void gui_log_flush_hand_over(CharDev* dev)
{

}

U8 gui_init_window_manager()
{
    // setup the memory block for the window list
    g_screen_resolution.horizontal = g_graphics_data.gop->Mode->Info->HorizontalResolution;
    g_screen_resolution.vertical = g_graphics_data.gop->Mode->Info->VerticalResolution;
    g_window_config.layer_index = memalloc(sizeof(U16) * g_window_config.window_available);
    g_window_config.window_available = 10;
    g_window_list = memalloc(sizeof(WindowLayerConfig) * g_window_config.window_available);
    g_window_config.window_in_use = 0;
    g_window_config.max_window_layer_in_use = g_window_config.window_in_use;
    g_window_config.window_focus_index = 0;
    for (U16 i = 0; i < g_window_config.window_available; i++)
    {
        // initialize the config
        g_window_list[i].in_use = 0;
        g_window_list[i].is_hide = 0;
        g_window_config.layer_index[i].window_in_use = 0;
    }
    // setup loading screen
    WindowData loading_screen_info_receiver;
    gui_window_manager_create_window(2, 0, &loading_screen_info_receiver);
    // Clear debug info, make screen ready for desktop
    graphics_clear_screen(0xFFFFFFFF);
    // overide the fbcon default kernel print
    // window 0 is defaultly for loading screen
    g_fbcon_node.con.max_x = ;
    g_fbcon_node.con.max_y = ;
    g_fbcon_node.con.current_y = 0;
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.con.common.putchar = gui_log_print_hand_over;
    g_fbcon_node.con.con.common.flush = gui_log_flush_hand_over;
}

U8 gui_trigger_loading_screen_status(U8 status)
{
    // status:  ==0: hide loading screen
    //          > 0: trigger loading screen
    g_gui_in_loading_screen = status;
    return 1;
}

U8 gui_window_manager_offline()
{
    // clear window draw buffer
    for (U16 i = 0; i < g_window_config.window_available; i++)
    {
        /* code */
        if (g_window_list[i].in_use)
        {
            memfree(g_window_list[i].base_info.frame_buffer_base);
        }
    }
    // clear window manager buffer
    memfree(g_window_list);
    memfree(g_window_config.layer_index);
    // clear screen
    graphics_clear_screen(0xFFFFFFFF);
    //reset fbcon to the default kernel print
    g_fbcon_node.con.max_x = g_graphics_data.pixels_per_line / 8 - 1;
    g_fbcon_node.con.max_y = g_graphics_data.gop->Mode->Info->VerticalResolution;
    g_fbcon_node.con.current_y = 0;
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.con.common.putchar = fbcon_putchar;
    g_fbcon_node.con.con.common.flush = fbcon_flush;
}

U8 gui_window_manager_create_window(U16 PID, U8 focus_mode, WindowData *info_receiver)
{
    // memory space still available
    U16 temp_window_index;
    g_window_config.window_in_use += 1;
    g_window_config.max_window_layer_in_use += 1;
    // can be improved later for performance enhancement
    for (U16 i = 0; i < g_window_config.window_available; i++)
    {
        // search for the first available window slot for this program
        if (g_window_list[i].in_use == 0)
        {
            break;
            temp_window_index = i;
        }
    }
    if (g_window_config.window_in_use == g_window_config.window_available)
    {
        // no enough space, realloc
        memrealloc(g_window_list, sizeof(WindowManageData) * (g_window_config.window_available + 10));
        memrealloc(g_window_config.layer_index, sizeof(WindowLayerConfig) * (g_window_config.window_available + 10));
        g_window_config.window_available += 10;
    }
    for (U16 i = g_window_config.window_in_use; i < g_window_config.window_available; i++)
    {
        // initialize the new config data
        g_window_list[i].in_use = 0;
        g_window_list[i].is_hide = 0;
        g_window_config.layer_index[i].window_in_use = 0;
    }
    g_window_list[temp_window_index].in_use = 1;
    g_window_list[temp_window_index].PID = PID;
    if (temp_window_index == 0 && PID == 2)
    {
        //loading screen
        g_window_list[temp_window_index].start_point_h = 0;
        g_window_list[temp_window_index].start_point_v = 0;
        g_window_list[temp_window_index].base_info.vertical = g_screen_resolution.horizontal;
        g_window_list[temp_window_index].base_info.horizontal = g_screen_resolution.vertical;
        // the loading/desktop screen is always at bottom, only triggered once
        g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_index = temp_window_index;
        g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_in_use = 1;
    }
    else
    {
        //normal procedure
        g_window_list[temp_window_index].start_point_h = 0.2 * g_screen_resolution.horizontal;
        g_window_list[temp_window_index].start_point_v = 0.2 * g_screen_resolution.vertical;
        g_window_list[temp_window_index].base_info.vertical = 0.6 * g_screen_resolution.horizontal;
        g_window_list[temp_window_index].base_info.horizontal = 0.6 * g_screen_resolution.vertical;
        if (focus_mode == 1)
        {
            g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_index = temp_window_index;
            g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_in_use = 1;
        }
        else
        {
            g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_index = g_window_config.layer_index[g_window_config.max_window_layer_in_use-2].window_index;
            g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_in_use = 1;
            g_window_config.layer_index[g_window_config.max_window_layer_in_use-2].window_index = temp_window_index;
        }
    }
    g_window_list[temp_window_index].base_info.frame_buffer_base = memalloc(sizeof(U32)*g_window_list[temp_window_index].base_info.vertical*g_window_list[temp_window_index].base_info.horizontal);
    info_receiver = g_window_list[temp_window_index];
    return 1; 
}

// this function will make check on the empty layer
U8 gui_window_manager_focus_change(U16 window_index)
{
    // change the focus here and reset the layer of all windows
    // avoid the empty layer when increase the new one
    if (g_window_list[window_index].in_use == 0 && window_index != 0)
    {
        // set focus failure, trigger error
        return 0;
    }
    else
    {
        // resort the layer here
        U16 temp_layer_index = 0;
        // ignore layer 0, which is loading screen
        for (U16 i = 1; i < g_window_config.max_window_layer_in_use; i++)
        {
            if ((g_window_config.layer_index[i].window_in_use) && (g_window_config.layer_index[i].window_index == window_index))
            {
                temp_layer_index = i;
                break;
            }
        }
        for (U16 i = temp_layer_index; i < (g_window_config.max_window_layer_in_use - 1); i++)
        {
            // set the higher level to overide the current
            g_window_config.layer_index[i].window_in_use = g_window_config.layer_index[i+1].window_in_use;
            g_window_config.layer_index[i].window_index = g_window_config.layer_index[i+1].window_index;
        }
        g_window_config.layer_index[g_window_config.max_window_layer_in_use].window_in_use = 1;
        g_window_config.layer_index[g_window_config.max_window_layer_in_use].window_index = window_index;
        //set focus index to current
        g_window_config.window_focus_index = window_index;
        return 1;
    }
}

U8 gui_window_manager_get_window_info(U16 PID, U16 window_index, WindowData *info_receiver)
{
    if (PID != g_window_list[window_index].PID)
    {
        // fail to verify, if system trigger force quit, read the PID in g_window_list
        return 0;
    }
    else
    {
        info_receiver = g_window_list[temp_window_index];
        return 1;
    }
}

U8 gui_window_manager_destory_window(U16 PID, U16 window_index)
{
    if (PID != g_window_list[window_index].PID)
    {
        // fail to verify, if system trigger force quit, read the PID in g_window_list
        return 0;
    }
    else
    {
        // trigger current window to top
        gui_window_manager_focus_change(window_index);
        // set the destoried layer to unused
        g_window_config.max_window_layer_in_use -= 1;
        g_window_config.layer_index[g_window_config.max_window_layer_in_use].window_in_use = 0;
        // change screen focus to next top layer
        g_window_config.window_focus_index = g_window_config.layer_index[g_window_config.max_window_layer_in_use-1].window_index;
        // clear the data/memory usage of destoried layer
        g_window_config.window_in_use -= 1;
        memfree(g_window_list[window_index].base_info.frame_buffer_base);
        g_window_list[window_index].in_use = 0;
        return 1;
    }
}