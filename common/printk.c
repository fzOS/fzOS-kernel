#include <common/printk.h>

void putU64hex(U64 data)
{
    U8 tempint;
    U8 temp1,temp2;
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
             g_default_console->common.putchar(&g_default_console->common,temp1);
        }else{
            temp1 = temp1 + 87;
             g_default_console->common.putchar(&g_default_console->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
             g_default_console->common.putchar(&g_default_console->common,temp2);
        }else{
            temp2 = temp2 + 87;
             g_default_console->common.putchar(&g_default_console->common,temp2);
        }
    }
}

void putU16hex(U16 data)
{
    U8 tempint;
    U8 temp1,temp2;
    for (int i = 0; i < 2; i++){
        /* code */
        tempint = (data&(0xff00 >> (8*i)) ) >> (8-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            g_default_console->common.putchar(&g_default_console->common,temp1);
        }else{
            temp1 = temp1 + 87;
            g_default_console->common.putchar(&g_default_console->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            g_default_console->common.putchar(&g_default_console->common,temp2);
        }else{
            temp2 = temp2 + 87;
            g_default_console->common.putchar(&g_default_console->common,temp2);
        }
    }
}
void putU8hex(U8 data)
{
    U8 temp1,temp2;
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
        g_default_console->common.putchar(&g_default_console->common,temp1);
    }else{
        temp1 = temp1 + 87;
        g_default_console->common.putchar(&g_default_console->common,temp1);
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
        g_default_console->common.putchar(&g_default_console->common,temp2);
    }else{
        temp2 = temp2 + 87;
        g_default_console->common.putchar(&g_default_console->common,temp2);
    }
}



void putstring(char *str)
{
    //为什么不递归呢？？？？
    printk(str);
}

void putnum(U64 num)
{
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        putnum((num/10));
    }
    g_default_console->common.putchar(&g_default_console->common,(char) tempint);
}

void putguid(GUID guid)
{
    //12345678-8765-4321-2333-666666666666
    putU16hex((guid.first&0xFFFF0000)>>16);
    putU16hex(guid.first&0xFFFF);
   g_default_console->common.putchar(&g_default_console->common,'-');
    putU16hex(guid.second);
   g_default_console->common.putchar(&g_default_console->common,'-');
    putU16hex(guid.third);
   g_default_console->common.putchar(&g_default_console->common,'-');
    for(int i=0;i<2;i++) {
        putU8hex(guid.fourth[i]);
    }
   g_default_console->common.putchar(&g_default_console->common,'-');
    for(int i=0;i<6;i++) {
        putU8hex(guid.fifth[i]);
    }
}
int printk(const char* format,...)
{
    int count=0;
    va_list arg;
    va_start(arg, format);
    const char* pointer = format;
    while(*pointer!='\0')
    {
        if(*pointer=='%')
        {
            count++;
            pointer++;
            switch(*pointer)
            {
                case 'c':{g_default_console->common.putchar(&g_default_console->common,va_arg(arg,int));break;}
                case 'd':{putnum(va_arg(arg,U64));break;}
                case 'x':{putU64hex(va_arg(arg,U64));break;}
                case 'b':{putU8hex(va_arg(arg,int));break;}
                case 'w':{putU16hex(va_arg(arg,int));break;}
                case 's':{putstring(va_arg(arg,char*));break;}
                case 'g':{putguid(va_arg(arg,GUID));break;}
                //我们支持全彩色了！
                //支持的颜色设置格式：
                //%#RRGGBB
                case '#':{
                    //只有屏幕终端显示颜色。
                    if(g_default_console->common.putchar!=fbcon_putchar) {
                        pointer+=6;
                        break;
                    }
                    //放入神奇的颜色转换代码！
                    fbcon_putchar(&g_default_console->common,COLOR_SWITCH_CHAR);
                    break;
                }
                default:{count--;break;}
            }
        }
        else
        {
            g_default_console->common.putchar(&g_default_console->common,*pointer);
        }
        pointer++;
    }
    va_end(arg);
    if(*(pointer-1)!=LINE_SEPARATOR) {
       g_default_console->common.flush(&g_default_console->common);
    }
    return count;
    
}
