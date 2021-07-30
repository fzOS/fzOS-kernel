#include <common/kstring.h>
#include <stdarg.h>
int printk(char*,...);
int strcopy(char* dest,char* src,int n) {
    int count=0;
    for(;count<n;count++) {
        dest[count]=src[count];
    }
    dest[n]=0;
    return ++count;
}
int strcomp(char* first,char* second) {
    int result =0;
    int pointer=0;
    while((!result)&&first[pointer]&&second[pointer]) {
        result = second[pointer]-first[pointer];
        pointer++;
    }
    if(!result)
        result=first[pointer]?-1:(second[pointer]?1:0);
    return result;
}
static void sputnum(char* buf,U64 num) {
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        sputnum(buf,(num/10));
    }
    *buf = ((char) tempint);
    buf++;
}


void sputU64hex(char* buf,U64 data)
{
    U8 tempint;
    U8 temp1,temp2;
    *buf = '0';
    buf++;
    *buf = 'x';
    buf++;
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            *buf = temp1;
            buf++;
        }else{
            temp1 = temp1 + 87;
            *buf = temp1;
            buf++;
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            *buf = temp1;
            buf++;
        }else{
            temp2 = temp2 + 87;
            *buf = temp1;
            buf++;
        }
    }
}
void sputU16hex(char* buf,U16 data)
{
    U8 tempint;
    U8 temp1,temp2;
    *buf = '0';
    buf++;
    *buf = 'x';
    buf++;
    for (int i = 0; i < 2; i++){
        /* code */
        tempint = (data&(0xff00 >> (8*i)) ) >> (8-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            *buf = temp1;
            buf++;
        }else{
            temp1 = temp1 + 87;
            *buf = temp1;
            buf++;
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            *buf = temp1;
            buf++;
        }else{
            temp2 = temp2 + 87;
            *buf = temp1;
            buf++;
        }
    }
}
void sputU8hex(char* buf,U8 data)
{
    U8 temp1,temp2;
    *buf = '0';
    buf++;
    *buf = 'x';
    buf++;
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
        *buf = temp1;
        buf++;
    }else{
        temp1 = temp1 + 87;
        *buf = temp1;
        buf++;
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
        *buf = temp1;
        buf++;
    }else{
        temp2 = temp2 + 87;
        *buf = temp1;
        buf++;
    }
}



void sputstring(char* buf,char *str)
{
    while (*str != '\0'){
        *buf=*str;
        buf++;
        str++;
    }
}
int sprintk(char* buf, const char* format,...)
{
    int count=0;
    va_list arg;
    va_start(arg, format);
    char* bufp = buf;
    const char* pointer = format;
    while(*pointer!='\0')
    {
        if(*pointer=='%')
        {
            count++;
            pointer++;
            switch(*pointer)
            {
                case 'c':{*bufp = *pointer;break;}
                case 'd':{sputnum(bufp,va_arg(arg,U64));break;}
                case 'x':{sputU64hex(bufp,va_arg(arg,U64));break;}
                case 'b':{sputU8hex(bufp,va_arg(arg,int));break;}
                case 'w':{sputU16hex(bufp,va_arg(arg,int));break;}
                case 's':{sputstring(bufp,va_arg(arg,char*));break;}
                default:{count--;break;}
            }
        }
        else
        {
            *bufp = *pointer;
        }
        pointer++;
        bufp++;
    }
    va_end(arg);
    *bufp='\0';
    return count;

}
