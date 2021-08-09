#include <common/printk.h>
void putU64hex(U64 data)
{
    U8 tempint;
    U8 temp1,temp2;
    default_console->common.putchar(&default_console->common,'0');
    default_console->common.putchar(&default_console->common,'x');
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
             default_console->common.putchar(&default_console->common,temp1);
        }else{
            temp1 = temp1 + 87;
             default_console->common.putchar(&default_console->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
             default_console->common.putchar(&default_console->common,temp2);
        }else{
            temp2 = temp2 + 87;
             default_console->common.putchar(&default_console->common,temp2);
        }
    }
}
void putU16hex(U16 data)
{
    U8 tempint;
    U8 temp1,temp2;
     default_console->common.putchar(&default_console->common,'0');
     default_console->common.putchar(&default_console->common,'x');
    for (int i = 0; i < 2; i++){
        /* code */
        tempint = (data&(0xff00 >> (8*i)) ) >> (8-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
             default_console->common.putchar(&default_console->common,temp1);
        }else{
            temp1 = temp1 + 87;
             default_console->common.putchar(&default_console->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
             default_console->common.putchar(&default_console->common,temp2);
        }else{
            temp2 = temp2 + 87;
             default_console->common.putchar(&default_console->common,temp2);
        }
    }
}
void putU8hex(U8 data)
{
    U8 temp1,temp2;
     default_console->common.putchar(&default_console->common,'0');
     default_console->common.putchar(&default_console->common,'x');
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
         default_console->common.putchar(&default_console->common,temp1);
    }else{
        temp1 = temp1 + 87;
         default_console->common.putchar(&default_console->common,temp1);
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
         default_console->common.putchar(&default_console->common,temp2);
    }else{
        temp2 = temp2 + 87;
         default_console->common.putchar(&default_console->common,temp2);
    }
}



void putstring(char *str)
{
    while (*str != '\0'){
         default_console->common.putchar(&default_console->common,*str);
        str++;
    }
}

void putnum(U64 num)
{
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        putnum((num/10));
    }
     default_console->common.putchar(&default_console->common,(char) tempint);
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
                case 'c':{ default_console->common.putchar(&default_console->common,va_arg(arg,int));break;}
                case 'd':{putnum(va_arg(arg,U64));break;}
                case 'x':{putU64hex(va_arg(arg,U64));break;}
                case 'b':{putU8hex(va_arg(arg,int));break;}
                case 'w':{putU16hex(va_arg(arg,int));break;}
                case 's':{putstring(va_arg(arg,char*));break;}
                default:{count--;break;}
            }
        }
        else
        {
             default_console->common.putchar(&default_console->common,*pointer);
        }
        pointer++;
    }
    va_end(arg);
    if(*(pointer-1)!=LINE_SEPARATOR) {
        default_console->common.flush(&default_console->common);
    }
    return count;
    
}
