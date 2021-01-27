#include <fbcon.h>
void fbcon_putchar(U8 c);
void fbcon_init(fbcon* buff)
{
    KERNEL_CONSOLE_FONT_MAX_X = graphics_data.pixels_per_line / 8 - 1;
    KERNEL_CONSOLE_FONT_MAX_Y = graphics_data.gop->Mode->Info->VerticalResolution;
    KERNEL_CONSOLE_FONT_POSITION_Y = 0;
    KERNEL_CONSOLE_FONT_POSITION_X = 0;
    buff->putchar = fbcon_putchar;
}
void kernel_display_move_up(){
    int upPix,newLine;
    upPix = (KERNEL_CONSOLE_FONT_POSITION_Y + 16) - graphics_data.gop->Mode->Info->VerticalResolution;
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
    KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y - upPix;
}
void kernel_log_line_break(){
    KERNEL_CONSOLE_FONT_POSITION_X = 0;
    KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 16;
    if (KERNEL_CONSOLE_FONT_MAX_Y < (KERNEL_CONSOLE_FONT_POSITION_Y + 16)){
        kernel_display_move_up();
    }
}
void fbcon_putchar(U8 c){
    if(c=='\n') {
        kernel_log_line_break();
        return;
    }
    unsigned const char* dots = fontdata_8x16 + c * 16;
    if ((KERNEL_CONSOLE_FONT_POSITION_X / 8) < KERNEL_CONSOLE_FONT_MAX_X){
        KERNEL_CONSOLE_FONT_POSITION_X = KERNEL_CONSOLE_FONT_POSITION_X + 8;
    }else{
        KERNEL_CONSOLE_FONT_POSITION_X = 0;
        KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 16;
    }
    if (KERNEL_CONSOLE_FONT_MAX_Y < (KERNEL_CONSOLE_FONT_POSITION_Y + 16)){
        kernel_display_move_up();
    }
    unsigned char data;	/*定义字符类型变量*/
    int bit;		/*定义bit，用来下方取出8位数组元素中的某一位*/
    int i,j;
    for(j=KERNEL_CONSOLE_FONT_POSITION_Y;j<KERNEL_CONSOLE_FONT_POSITION_Y+16;j++)
    {
        data=*dots++;		/*指向下一个字符*/
        bit = 7;			/*从最高位第七位开始*/
        for(i=KERNEL_CONSOLE_FONT_POSITION_X;i<KERNEL_CONSOLE_FONT_POSITION_X+8;i++)		/*从高到底显示出8位的每一位*/
        {
                if(data&(1<<bit)){
                    graphics_draw_pixel(i,j,0xFFFFFFFF);	/*调用显示点函数，在此位置显示点*/
                }
                bit--;			/*指向低一位*/
        }
    }
}
