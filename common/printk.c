#include <common/printk.h>
void putU64hex(U64 data)
{
    U8 tempint;
    U8 temp1,temp2;
    stdio.putchar('0');
    stdio.putchar('x');
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            stdio.putchar(temp1);
        }else{
            temp1 = temp1 + 87;
            stdio.putchar(temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            stdio.putchar(temp2);
        }else{
            temp2 = temp2 + 87;
            stdio.putchar(temp2);
        }
    }
}
void putU16hex(U16 data)
{
    U8 tempint;
    U8 temp1,temp2;
    stdio.putchar('0');
    stdio.putchar('x');
    for (int i = 0; i < 2; i++){
        /* code */
        tempint = (data&(0xff00 >> (8*i)) ) >> (8-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            stdio.putchar(temp1);
        }else{
            temp1 = temp1 + 87;
            stdio.putchar(temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            stdio.putchar(temp2);
        }else{
            temp2 = temp2 + 87;
            stdio.putchar(temp2);
        }
    }
}
void putU8hex(U8 data)
{
    U8 temp1,temp2;
    stdio.putchar('0');
    stdio.putchar('x');
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
        stdio.putchar(temp1);
    }else{
        temp1 = temp1 + 87;
        stdio.putchar(temp1);
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
        stdio.putchar(temp2);
    }else{
        temp2 = temp2 + 87;
        stdio.putchar(temp2);
    }
}



void putstring(char *str)
{
    while (*str != '\0'){
        stdio.putchar(*str);        
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
    stdio.putchar((char) tempint);
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
                case 'c':{stdio.putchar(va_arg(arg,int));break;}
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
            stdio.putchar(*pointer);
        }
        pointer++;
    }
    va_end(arg);
    return count;
    
}
