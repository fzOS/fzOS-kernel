#include <zcrystal/window_manager.h>
#include <drivers/graphics.h>
#include <memory/memory.h>
#include <types.h>

ScreenDefinition g_screen_resolution;
// window list
WindowManageData *g_window_list;
// in use and available
WindowManageConfig g_window_config;

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
    g_window_list = memalloc(sizeof(g_window_list) * 10);
    g_window_config.window_available = 10;
    g_window_config.window_in_use = 0;
    g_window_config.window_focus_index = 0;
    for (U16 i = 0; i < g_window_config.window_available; i++)
    {
        // initialize the config
        g_window_list[i].in_use = 0;
    }
    
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
    // status:  0: hide loading screen
    //          1: trigger loading screen
}

U8 gui_window_manager_offline()
{
    // clear window draw buffer
    for (U16 i = 0; i < g_window_config.window_in_use; i++)
    {
        /* code */
        memfree(g_window_list[i].base_info.frame_buffer_base);
    }
    // clear window manager buffer
    memfree(g_window_list);
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
    if (g_window_config.window_available < g_window_config.window_in_use)
    {
        // memory space still available
        U16 temp_window_index;
        g_window_config.window_in_use += 1;
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
        g_window_list[temp_window_index].in_use = 1;
        g_window_list[temp_window_index].PID = PID;
        g_window_list[temp_window_index].start_point_h = 0.2 * g_screen_resolution.horizontal;
        g_window_list[temp_window_index].start_point_v = 0.2 * g_screen_resolution.vertical;
        if (focus_mode == 1)
        {

            
        }
    }
}

// this function will make check on the empty layer
U8 gui_window_manager_focus_change(U16 window_index)
{
    // change the focus here and reset the layer of all windows
    // avoid the empty layer when increase the new one
    if (g_window_config.current_max_layer > g_window_config.window_in_use)
    {
        
    }
}