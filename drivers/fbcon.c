#include <drivers/fbcon.h>
#include <memory/memory.h>
#include <drivers/asciifont.h>
void fbcon_putchar(CharDev* dev, U8 c);
//定义一个特殊的结点。
//因为设备树还没初始化，所以只能这么写了。
FbconDeviceTreeNode g_fbcon_node;
Console* g_default_console = &g_fbcon_node.con.con;
void fbcon_init(void)
{
    g_fbcon_node.con.max_x = g_graphics_data.pixels_per_line / 8 - 1;
    g_fbcon_node.con.max_y = g_graphics_data.gop->Mode->Info->VerticalResolution;
    g_fbcon_node.con.color = 0xFFFFFFFF;
    g_fbcon_node.con.current_y = 0;
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.con.common.putchar = fbcon_putchar;
    g_fbcon_node.con.con.common.flush = fbcon_flush;
    g_fbcon_node.con.con.input_buffer.queue.bufsize = BUFFER_MAX;
    g_fbcon_node.con.con.output_buffer.queue.bufsize = BUFFER_MAX;
    g_fbcon_node.node.type = DT_CHAR_DEVICE;
    memcpy(g_fbcon_node.node.name,"Console",8);
}
void fbcon_set_color(U32 color)
{
    g_fbcon_node.con.color = color;
}
void kernel_display_move_up()
{
    int upPix = (g_fbcon_node.con.current_y + 16) - g_graphics_data.gop->Mode->Info->VerticalResolution;
    graphics_move_up(upPix);
    g_fbcon_node.con.current_y = g_fbcon_node.con.current_y - upPix;
}
void kernel_log_line_break()
{
    g_fbcon_node.con.current_x = 0;
    g_fbcon_node.con.current_y = g_fbcon_node.con.current_y + 16;
    if (g_fbcon_node.con.max_y < (g_fbcon_node.con.current_y + 16)){
        kernel_display_move_up();
    }
}
void fbcon_putchar(CharDev* dev, U8 c)
{
    (void) dev;
    queue_in_single(&g_fbcon_node.con.con.output_buffer.queue,c);
    release_semaphore(&g_fbcon_node.con.con.output_sem);
    if(c==LINE_SEPARATOR||g_fbcon_node.con.con.output_sem>g_fbcon_node.con.con.output_buffer.queue.bufsize) {
        fbcon_flush(dev);
    }
}

inline U8 char2u8(char c) {
    if ((c >= 'a') && (c <= 'f')) {
        c -= 32;
    }
    if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    }
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }
    else {
        return 255;//不是一个有效的数位。
    }
}

void fbcon_flush(CharDev* dev) {
    (void) dev;
    char c;
    if(g_fbcon_node.con.output_in_progress) {
        //已经在执行刷新工作;退出。
        return;
    }
    release_semaphore(&g_fbcon_node.con.output_in_progress);
    while(g_fbcon_node.con.con.output_sem>0) {
        c = queue_out_single(&g_fbcon_node.con.con.output_buffer.queue);
        acquire_semaphore(&g_fbcon_node.con.con.output_sem);
        if(c==LINE_SEPARATOR) {
            kernel_log_line_break();
            break;
        }
        if(c==COLOR_SWITCH_CHAR) {
            U32 color=0;
            U8 d;
            //切换颜色！
            for(int i=0;i<6;i++) {
                c= queue_out_single(&g_fbcon_node.con.con.output_buffer.queue);
                acquire_semaphore(&g_fbcon_node.con.con.output_sem);
                d = char2u8(c);
                if(d==255) {
                    goto normal_print_char;
                }
                color |= (d<<((5-i)*4));
            }
            fbcon_set_color(color);
            continue;
        }
normal_print_char:;
        unsigned const char* dots = FONTDATA_8x16 + c * 16;
        if ((g_fbcon_node.con.current_x / 8) < g_fbcon_node.con.max_x){
            g_fbcon_node.con.current_x = g_fbcon_node.con.current_x + 8;
        }else{
            g_fbcon_node.con.current_x = 0;
            g_fbcon_node.con.current_y = g_fbcon_node.con.current_y + 16;
        }
        if (g_fbcon_node.con.max_y < (g_fbcon_node.con.current_y + 16)){
            kernel_display_move_up();
        }
        unsigned char data;	/*定义字符类型变量*/
        int bit;		/*定义bit，用来下方取出8位数组元素中的某一位*/
        int i,j;
        for(j=g_fbcon_node.con.current_y;j<g_fbcon_node.con.current_y+16;j++)
        {
            data=*dots++;		/*指向下一个字符*/
            bit = 7;			/*从最高位第七位开始*/
            for(i=g_fbcon_node.con.current_x;i<g_fbcon_node.con.current_x+8;i++)		/*从高到底显示出8位的每一位*/
            {
                    if(data&(1<<bit)){
                        graphics_draw_pixel(i,j,g_fbcon_node.con.color);	/*调用显示点函数，在此位置显示点*/
                    }
                    bit--;			/*指向低一位*/
            }
        }
    }
    acquire_semaphore(&g_fbcon_node.con.output_in_progress);
}
void fbcon_add_to_device_tree(void)
{
    DeviceTreeNode* base_node = device_tree_resolve_by_path(BASE_DEVICE_TREE_TEMPLATE,nullptr,DT_CREATE_IF_NONEXIST);
    device_tree_add_from_parent(&g_fbcon_node.node,base_node);
}
