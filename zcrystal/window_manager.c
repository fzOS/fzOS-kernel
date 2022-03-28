#include <drivers/graphics.h>
#include <drivers/fbcon.h>
#include <drivers/chardev.h>
#include <memory/memory.h>
#include <types.h>

#include <zcrystal/window_manager.h>
#include <zcrystal/render.h>
extern FbconDeviceTreeNode g_fbcon_node;
ScreenDefinition g_screen_resolution;
// 双向链表
WindowManageData *g_window_list_top;
WindowManageData *g_window_list_bottom;

U32 g_gui_loading_window_uid;
U32 g_gui_current_max_uid = 0;

U8 g_gui_in_loading_screen = 1;

void gui_log_print_hand_over(CharDev* dev, U8 c)
{
    
}

void gui_log_flush_hand_over(CharDev* dev)
{

}

U8 gui_init_window_manager(int gui_aero_enable)
{
    g_gui_current_max_uid = 0;
    // setup the memory block for the window list
    g_screen_resolution.horizontal = g_graphics_data.gop->Mode->Info->HorizontalResolution;
    g_screen_resolution.vertical = g_graphics_data.gop->Mode->Info->VerticalResolution;
    // set up loading screen (also use as desktop bg)
    g_window_list_bottom = memalloc(sizeof(WindowManageData));
    // link top window to the bottom too
    g_window_list_top = g_window_list_bottom;
    // initial the doubly linked list
    g_window_list_bottom->next = NULL;
    g_window_list_bottom->prev = NULL;
    // setup window info
    g_window_list_bottom->start_point_h = 0;
    g_window_list_bottom->start_point_v = 0;
    g_window_list_bottom->is_hide = 0;
    // use pid 0 to get the window
    g_window_list_bottom->PID = 0;

    // initialize the first window(background)
    WindowDataExport loading_screen_info_receiver;
    g_gui_loading_window_uid = gui_window_manager_create_window(0, WINDOW_STATUS_LOADING, WINDOW_MODE_BORDERLESS, 0, 0, 0, 0, &loading_screen_info_receiver);
    // Clear debug info, make screen ready for desktop
    graphics_clear_screen(0xFFFFFFFF);
    // overide the fbcon default kernel print
    // window 0 is defaultly for loading screen
    //g_fbcon_node.con.max_x = ;
    //g_fbcon_node.con.max_y = ;
    g_fbcon_node.con.current_y = 0;
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.con.common.putchar = gui_log_print_hand_over;
    g_fbcon_node.con.con.common.flush = gui_log_flush_hand_over;
    return 1;
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
    // clear window draw buffer and the window manager itself
    WindowManageData* temp_pointer, temp_pointer_next;
    (void)temp_pointer_next;
    temp_pointer = g_window_list_top;
    while (temp_pointer->next != NULL)
    {
        temp_pointer_next = *temp_pointer->next;
        gui_window_manager_destroy_window(temp_pointer->PID, temp_pointer->UID);
    }
    // clear screen
    graphics_clear_screen(0xFFFFFFFF);
    //reset fbcon to the default kernel print
    g_fbcon_node.con.max_x = g_graphics_data.pixels_per_line / 8 - 1;
    g_fbcon_node.con.max_y = g_graphics_data.gop->Mode->Info->VerticalResolution;
    g_fbcon_node.con.current_y = 0;
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.con.common.putchar = fbcon_putchar;
    g_fbcon_node.con.con.common.flush = fbcon_flush;
    return 0;
}

U8 gui_window_manager_create_window(U16 PID, U8 focus_status, U8 window_mode, U16 pos_h, U16 pos_v, U16 size_h, U16 size_v, WindowDataExport *info_receiver)
{
    // overrided: 1 for focus at top layer, 0 for trigger lower than it, 2 for initial use
    U8 focus_mode, hidden_status;
    hidden_status = 0;
    focus_mode = 0;
    switch (focus_status)
    {
    case WINDOW_STATUS_HIDDEN:
        hidden_status = 1;
    case WINDOW_STATUS_INACTIVE:
        focus_mode = 0;
        break;
    case WINDOW_STATUS_NORMAL:
        focus_mode = 1;
        break;
    case WINDOW_STATUS_LOADING:
        focus_mode = 2;
        break;
    default:
        break;
    }

    WindowManageData* temp_pointer;
    // depends on if it is the initial process
    if (focus_mode != 2)
    {
        // not initial, create new space for window
        temp_pointer = memalloc(sizeof(WindowManageData));
    }
    else
    {
        temp_pointer = g_window_list_top;
    }

    temp_pointer->window_bar_mode = window_mode;

    temp_pointer->UID = g_gui_current_max_uid;
    g_gui_current_max_uid += 1;
    temp_pointer->PID = PID;
    temp_pointer->is_hide = hidden_status;
    if (focus_mode == 2 && PID == 0)
    {
        //loading screen
        temp_pointer->start_point_h = 0;
        temp_pointer->start_point_v = 0;
        temp_pointer->base_info.vertical = g_screen_resolution.vertical;
        temp_pointer->base_info.horizontal = g_screen_resolution.horizontal;
    }
    else
    {
        //normal procedure
        temp_pointer->start_point_h = pos_h;
        temp_pointer->start_point_v = pos_v;
        temp_pointer->base_info.vertical = size_v + 30;
        temp_pointer->base_info.horizontal = size_h;
    }
    temp_pointer->base_info.frame_buffer_base = memalloc(sizeof(U32)* (temp_pointer->base_info.vertical) * (temp_pointer->base_info.horizontal));
    temp_pointer->base_info.frame_buffer_base_User = temp_pointer->base_info.frame_buffer_base + 30 * (temp_pointer->base_info.horizontal);
    // *info_receiver = *temp_pointer->base_info;
    // if it is the normal window, loading window will ignore this
    WindowData tempWindowData;
    tempWindowData = temp_pointer->base_info;
    U8 is_loading = 0;
    if (focus_mode == 2)
    {
        is_loading = 1;
    }
    gui_render_preset_window(&tempWindowData, is_loading, window_mode);

    if (focus_mode == 1 || (g_window_list_top->next == NULL && focus_mode != 2))
    {
        temp_pointer->prev = NULL;
        temp_pointer->next = g_window_list_top;
        tempWindowData = g_window_list_top->base_info;
        gui_render_application_bar_status_change(&tempWindowData, 0);
        g_window_list_top->prev = temp_pointer;
        g_window_list_top = temp_pointer;
    }
    else if (focus_mode == 0)
    {
        // add current window below the top layer
        temp_pointer->prev = g_window_list_top;
        temp_pointer->next = g_window_list_top->next;
        g_window_list_top->next = temp_pointer;
        tempWindowData = temp_pointer->base_info;
        gui_render_application_bar_status_change(&tempWindowData, 0);
    }
    
    info_receiver->horizontal = temp_pointer->base_info.horizontal;
    info_receiver->vertical = temp_pointer->base_info.vertical - 30;
    info_receiver->frame_buffer_base = temp_pointer->base_info.frame_buffer_base_User;
    return 1; 
}

WindowManageData* gui_window_manager_get_window_pointer(U32 unique_id)
{
    WindowManageData* temp_pointer;
    temp_pointer = g_window_list_top;
    while (temp_pointer->next != NULL)
    {
        if (temp_pointer->UID == unique_id)
        {
            return temp_pointer;
            break;
        }
        else
        {
            temp_pointer = temp_pointer->next;
        }
    }
    return NULL;
}

U8 gui_window_manager_focus_change(U32 unique_id)
{
    WindowManageData* temp_pointer = NULL;
    WindowManageData* temp_pointer2 = NULL;
    temp_pointer = gui_window_manager_get_window_pointer(unique_id);
    // switch the color
    WindowData tempWindowData = g_window_list_top->base_info;
    gui_render_application_bar_status_change(&tempWindowData, 0);
    tempWindowData = temp_pointer->base_info;
    gui_render_application_bar_status_change(&tempWindowData, 1);
    if (temp_pointer == NULL)
    {
        return 0;
    }
    else
    {   
        temp_pointer2 = temp_pointer->prev;
        temp_pointer2->next = temp_pointer->next;
        temp_pointer2 = temp_pointer->next;
        temp_pointer2->prev = temp_pointer->prev;
        temp_pointer->prev = NULL;
        temp_pointer->next = g_window_list_top;
        g_window_list_top->prev = temp_pointer;
        return 1;
    }
}

U8 gui_window_manager_get_window_info(U16 PID, U32 unique_id, WindowDataExport *info_receiver)
{
    WindowManageData* temp_pointer = NULL;
    temp_pointer = gui_window_manager_get_window_pointer(unique_id);
    if (temp_pointer != NULL && temp_pointer->PID == PID)
    {
        //FIXME:What is this?
        info_receiver = ( WindowDataExport*)&(temp_pointer->base_info);
        if (unique_id > 0)
        {
            // if it is the normal window
            info_receiver->horizontal = temp_pointer->base_info.horizontal;
            // the top 30 are default for head bar
            info_receiver->vertical = temp_pointer->base_info.vertical - 30;
            info_receiver->frame_buffer_base = temp_pointer->base_info.frame_buffer_base_User;
        }
        
        return 1;
    }
    else
    {
        return 0;
    }
}

U8 gui_loading_screen_request(WindowDataExport *info_receiver)
{
    WindowManageData* temp_pointer = NULL;
    U32 unique_id = 0;
    //FIXME:WHERE IS PID?
    U32 PID=0;
    temp_pointer = gui_window_manager_get_window_pointer(unique_id);
    if (temp_pointer != NULL && temp_pointer->PID == PID)
    {
        //FIXME:WHERE IS THIS?
        info_receiver = (WindowDataExport *)&(temp_pointer->base_info);
        if (unique_id > 0)
        {
            // if it is the normal window
            info_receiver->horizontal = temp_pointer->base_info.horizontal;
            // the top 30 are default for head bar
            info_receiver->vertical = temp_pointer->base_info.vertical;
            info_receiver->frame_buffer_base = temp_pointer->base_info.frame_buffer_base;
        }
        
        return 1;
    }
    else
    {
        return 0;
    }
}

U8 gui_window_manager_destroy_window(U16 PID, U32 unique_id)
{
    WindowManageData* temp_pointer = NULL;
    temp_pointer = gui_window_manager_get_window_pointer(unique_id);
    if (temp_pointer != NULL && temp_pointer->PID == PID)
    {
        //start destory process
        WindowManageData* temp_pointer2 = NULL;
        // reconnect linked list
        if (temp_pointer->prev == NULL)
        {
            // this is top layer
            g_window_list_top = temp_pointer->next;
        }
        else
        {
            temp_pointer2 = temp_pointer->prev;
            temp_pointer2->next = temp_pointer->next;
        }
        // should never reach here, unless it is killing the desktop/loading window
        if (temp_pointer->next == NULL)
        {
            // this is top layer
            g_window_list_bottom = temp_pointer->prev;
        }
        else
        {
            temp_pointer2 = temp_pointer->next;
            *temp_pointer2->prev = *temp_pointer->prev;
        }
        // free the window screen buffer
        memfree(temp_pointer->base_info.frame_buffer_base);
        // finally clean this block of memory
        memfree(temp_pointer);
        return 1;
    }
    else
    {
        return 0;
    }
}
