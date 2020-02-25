#include "asciifont.c"

#include <kerneldisplay.h>

void kernel_draw_char_pos(U32 color){
    int x,y;
    if (KERNEL_CONSOLE_FONT_POSITION_X < KERNEL_CONSOLE_FONT_MAX_X){
        x = KERNEL_CONSOLE_FONT_POSITION_X * 8;
        y = KERNEL_CONSOLE_FONT_POSITION_Y * 16;
        KERNEL_CONSOLE_FONT_POSITION_X = KERNEL_CONSOLE_FONT_POSITION_X + 1;
    }else{
        KERNEL_CONSOLE_FONT_POSITION_X = 0;
        KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 1;
        x = KERNEL_CONSOLE_FONT_POSITION_X * 8;
        y = KERNEL_CONSOLE_FONT_POSITION_Y * 16;
    }
    graphics_fill_rect(x, y, 8, 16, color);
}

void kernel_print_U64(U64 data, U32 color){
    
}

void kernel_log_print_char(char c,U32 color){
    unsigned const char* dots = fontdata_8x16 + c * 16;
    int x,y;
    if (KERNEL_CONSOLE_FONT_POSITION_X < KERNEL_CONSOLE_FONT_MAX_X){
        x = KERNEL_CONSOLE_FONT_POSITION_X * 8;
        y = KERNEL_CONSOLE_FONT_POSITION_Y * 16;
        KERNEL_CONSOLE_FONT_POSITION_X = KERNEL_CONSOLE_FONT_POSITION_X + 1;
    }else{
        KERNEL_CONSOLE_FONT_POSITION_X = 0;
        KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 1;
        x = KERNEL_CONSOLE_FONT_POSITION_X * 8;
        y = KERNEL_CONSOLE_FONT_POSITION_Y * 16;
    }
    unsigned char data;	/*定义字符类型变量*/
    int bit;		/*定义bit，用来下方取出8位数组元素中的某一位*/
    int i,j;
    for(j=y;j<y+16;j++)
    {
        data=*dots++;		/*指向下一个字符*/
        bit = 7;			/*从最高位第七位开始*/
        for(i=x;i<x+8;i++)		/*从高到底显示出8位的每一位*/
        {
                if(data&(1<<bit)){
                    graphics_draw_pixel(i,j,color);	/*调用显示点函数，在此位置显示点*/
                }
                bit--;			/*指向低一位*/
        }
    }
}

void kernel_log_line_break(){
    KERNEL_CONSOLE_FONT_POSITION_X = 0;
    KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 1;
}

void kernel_log_print_string(char *str,U32 color){
    while (*str != '\0'){
        if (*str != '\n'){
            kernel_log_print_char(*str,color);
        }else{
            kernel_log_line_break();
        }
        
        str++;
    }
}