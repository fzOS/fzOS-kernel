#include "asciifont.c"

#include <kerneldisplay.h>

void kernel_draw_char_pos(U32 color){
    if ((KERNEL_CONSOLE_FONT_POSITION_X / 8) < KERNEL_CONSOLE_FONT_MAX_X){
        KERNEL_CONSOLE_FONT_POSITION_X = KERNEL_CONSOLE_FONT_POSITION_X + 8;
    }else{
        KERNEL_CONSOLE_FONT_POSITION_X = 0;
        KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 16;
    }
    graphics_fill_rect(KERNEL_CONSOLE_FONT_POSITION_X, KERNEL_CONSOLE_FONT_POSITION_Y, 8, 16, color);
}

void kernel_print_U64_hex(U64 data, U32 color){
    U8 tempint;
    U8 temp1,temp2;
    kernel_log_print_char('0', color);
    kernel_log_print_char('x', color);
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            kernel_log_print_char(temp1, color);
        }else{
            temp1 = temp1 + 87;
            kernel_log_print_char(temp1, color);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            kernel_log_print_char(temp2, color);
        }else{
            temp2 = temp2 + 87;
            kernel_log_print_char(temp2, color);
        }
    }
    kernel_log_print_char(' ', color);
}

void kernel_print_U8_hex(U8 data, U32 color){
    U8 temp1,temp2;
    kernel_log_print_char('0', color);
    kernel_log_print_char('x', color);
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
        kernel_log_print_char(temp1, color);
    }else{
        temp1 = temp1 + 87;
        kernel_log_print_char(temp1, color);
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
        kernel_log_print_char(temp2, color);
    }else{
        temp2 = temp2 + 87;
        kernel_log_print_char(temp2, color);
    }
    kernel_log_print_char(' ', color);
}

void kernel_log_print_char(char c,U32 color){
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
                    graphics_draw_pixel(i,j,color);	/*调用显示点函数，在此位置显示点*/
                }
                bit--;			/*指向低一位*/
        }
    }
}

void kernel_log_line_break(){
    KERNEL_CONSOLE_FONT_POSITION_X = 0;
    KERNEL_CONSOLE_FONT_POSITION_Y = KERNEL_CONSOLE_FONT_POSITION_Y + 16;
    if (KERNEL_CONSOLE_FONT_MAX_Y < (KERNEL_CONSOLE_FONT_POSITION_Y + 16)){
        kernel_display_move_up();
    }
}

void kernel_log_print_string(char *str,U32 color){
    while (*str != '\0'){
        kernel_log_print_char(*str,color);        
        str++;
    }
}

void kernel_log_print_line_cut(char c, U32 color){
    kernel_log_line_break();
    while ((KERNEL_CONSOLE_FONT_POSITION_X / 8) < (KERNEL_CONSOLE_FONT_MAX_X - 8)){
        kernel_log_print_char(c, color);
    }
}

void kernel_log_print_num(U64 num, U32 color){
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        kernel_log_print_num((num/10), color);
    }
    kernel_log_print_char((char) tempint, color);
}
int printk(char* format,...)
{
    int count=0;
    va_list arg;
    va_start(arg, format);
    char* pointer = format;
    while(*pointer!='\0')
    {
        if(*pointer=='%')
        {
            count++;
            pointer++;
            switch(*pointer)
            {
                case 'c':{kernel_log_print_char(va_arg(arg,int),0x00ffffff);break;}
                case 'd':{kernel_log_print_num(va_arg(arg,U64),0x00ffffff);break;}
                case 'x':{kernel_print_U64_hex(va_arg(arg,U64),0x00ffffff);break;}
                case 'b':{kernel_print_U8_hex(va_arg(arg,int),0x00ffffff);break;}
                case 's':{kernel_log_print_string(va_arg(arg,char*),0x00ffffff);break;}
                default:{count--;break;}
            }
            //pointer++;
        }
        else
        {
            kernel_log_print_char(*pointer,0x00ffffff);
        }
        pointer++;
    }
    return count;
    
}