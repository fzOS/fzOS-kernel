#include <drivers/fbcon.h>
#include <memory/memory.h>

void fbcon_putchar(char_dev* dev, U8 c);
//定义一个特殊的结点。
//因为设备树还没初始化，所以只能这么写了。
fbcon_device_tree_node fbcon_node;
console* default_console = &fbcon_node.con.con;
void fbcon_init(void)
{
    fbcon_node.con.max_x = graphics_data.pixels_per_line / 8 - 1;
    fbcon_node.con.max_y = graphics_data.gop->Mode->Info->VerticalResolution;
    fbcon_node.con.current_y = 0;
    fbcon_node.con.current_x = 0;
    fbcon_node.con.con.common.putchar = fbcon_putchar;
    fbcon_node.con.con.input_buffer.queue.bufsize = BUFFER_MAX;
    fbcon_node.con.con.output_buffer.queue.bufsize = BUFFER_MAX;
    fbcon_node.node.type = DT_CHAR_DEVICE;
    memcpy(fbcon_node.node.name,"Console",8);
}
void kernel_display_move_up(){
    int upPix,newLine;
    upPix = (fbcon_node.con.current_y + 16) - graphics_data.gop->Mode->Info->VerticalResolution;
    newLine = graphics_data.gop->Mode->Info->VerticalResolution - upPix;
    for (int i = 0; i < graphics_data.gop->Mode->Info->VerticalResolution; i++){
        for (int j = 0; j < graphics_data.gop->Mode->Info->HorizontalResolution; j++){
            if (i >= newLine){
                graphics_data.frame_buffer_base[i * graphics_data.pixels_per_line + j] = graphics_data.default_background_color;
            }else{
                graphics_data.frame_buffer_base[i * graphics_data.pixels_per_line + j] = graphics_data.frame_buffer_base[(i + upPix) * graphics_data.pixels_per_line + j];
            }
        }
    }
    fbcon_node.con.current_y = fbcon_node.con.current_y - upPix;
}
void kernel_log_line_break(){
    fbcon_node.con.current_x = 0;
    fbcon_node.con.current_y = fbcon_node.con.current_y + 16;
    if (fbcon_node.con.max_y < (fbcon_node.con.current_y + 16)){
        kernel_display_move_up();
    }
}
void fbcon_putchar(char_dev* dev, U8 c){
    (void) dev;
    if(c=='\n') {
        kernel_log_line_break();
        return;
    }
    unsigned const char* dots = fontdata_8x16 + c * 16;
    if ((fbcon_node.con.current_x / 8) < fbcon_node.con.max_x){
        fbcon_node.con.current_x = fbcon_node.con.current_x + 8;
    }else{
        fbcon_node.con.current_x = 0;
        fbcon_node.con.current_y = fbcon_node.con.current_y + 16;
    }
    if (fbcon_node.con.max_y < (fbcon_node.con.current_y + 16)){
        kernel_display_move_up();
    }
    unsigned char data;	/*定义字符类型变量*/
    int bit;		/*定义bit，用来下方取出8位数组元素中的某一位*/
    int i,j;
    for(j=fbcon_node.con.current_y;j<fbcon_node.con.current_y+16;j++)
    {
        data=*dots++;		/*指向下一个字符*/
        bit = 7;			/*从最高位第七位开始*/
        for(i=fbcon_node.con.current_x;i<fbcon_node.con.current_x+8;i++)		/*从高到底显示出8位的每一位*/
        {
                if(data&(1<<bit)){
                    graphics_draw_pixel(i,j,0xFFFFFFFF);	/*调用显示点函数，在此位置显示点*/
                }
                bit--;			/*指向低一位*/
        }
    }
}
void fbcon_add_to_device_tree(void)
{
    device_tree_node* base_node = device_tree_resolve_by_path(BASE_DEVICE_TREE_TEMPLATE,nullptr,DT_CREATE_IF_NONEXIST);
    device_tree_add_from_parent(&fbcon_node.node,base_node);
}
