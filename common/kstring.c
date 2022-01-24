#include <common/kstring.h>
#include <stdarg.h>
int printk(char*,...);
int strcopy(char* dest,char* src,int n) {
    int count=0;
    for(;count<n;count++) {
        dest[count]=src[count];
        if(dest[count]=='\0') {
            break;
        }
    }
    dest[count]=0;
    return ++count;
}
int strcomp(const char* first,const char* second) {
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
static int sputnum(char* buf,U64 num) {
    char* bufp = buf;
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        sputnum(buf,(num/10));
    }
    *buf = ((char) tempint);
    buf++;
    return buf-bufp;
}


static int sputU64hex(char* buf,U64 data)
{
    char* bufp = buf;
    U8 tempint;
    U8 temp1,temp2;
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
            *buf = temp2;
            buf++;
        }else{
            temp2 = temp2 + 87;
            *buf = temp2;
            buf++;
        }
    }
    return buf-bufp;
}
static int sputU16hex(char* buf,U16 data)
{
    char* bufp = buf;
    U8 tempint;
    U8 temp1,temp2;
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
            *buf = temp2;
            buf++;
        }else{
            temp2 = temp2 + 87;
            *buf = temp2;
            buf++;
        }
    }
    return buf-bufp;
}
static int sputU8hex(char* buf,U8 data)
{

    char* bufp = buf;
    U8 temp1,temp2;
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
        *buf = temp2;
        buf++;
    }else{
        temp2 = temp2 + 87;
        *buf = temp2;
        buf++;
    }
    return buf-bufp;
}
static int sputguid(char* buf,GUID guid)
{
    char* bufp = buf;
    int off;
    off=sputU16hex(bufp,(guid.first&0xFFFF0000)>>16);
    bufp+=off;
    off=sputU16hex(bufp,(guid.first&0xFFFF));
    bufp+=off;
    *bufp='-';
    bufp++;
    off=sputU16hex(bufp,guid.second);
    bufp+=off;
    *bufp='-';
    bufp++;
    off=sputU16hex(bufp,guid.third);
    bufp+=off;
    *bufp='-';
    bufp++;
    for(int i=0;i<2;i++) {
        off=sputU8hex(bufp,guid.fourth[i]);
        bufp+=off;
    }
    *bufp='-';
    bufp++;
    for(int i=0;i<6;i++) {
        off=sputU8hex(bufp,guid.fifth[i]);
        bufp+=off;
    }
    return bufp-buf;
}
static int sputstring(char* buf,char *str)
{
    char* bufp = buf;
    while (*str != '\0'){
        *buf=*str;
        buf++;
        str++;
    }
    return buf-bufp;
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
                case 'c':{*bufp = *pointer;bufp++;break;}
                case 'd':{bufp+=sputnum(bufp,va_arg(arg,U64));break;}
                case 'x':{bufp+=sputU64hex(bufp,va_arg(arg,U64));break;}
                case 'b':{bufp+=sputU8hex(bufp,va_arg(arg,int));break;}
                case 'w':{bufp+=sputU16hex(bufp,va_arg(arg,int));break;}
                case 's':{bufp+=sputstring(bufp,va_arg(arg,char*));break;}
                case 'g':{bufp+=sputguid(bufp,va_arg(arg,GUID));break;}
                default:{count--;break;}
            }
        }
        else
        {
            *bufp = *pointer;
            bufp++;
        }
        pointer++;
    }
    va_end(arg);
    *bufp='\0';
    return count;

}
char* strchr(char* str,char delim)
{
    char* p = str;
    while(*p!='\0' && *p!=delim) {
        p++;
    }
    if(*p=='\0') {
        p = nullptr;
    }
    return p;
}
int strmid(char* buffer,int buflen,char* last_position,char delim)
{
    char* p = last_position;
    while(*p!='\0' && *p!=delim) {
        p++;
    }
    int len = p-last_position;
    if(len>=buflen) {
        return FzOS_BUFFER_TOO_SMALL;
    }
    strcopy(buffer,last_position,len);
    return len;
}
U64 shrink_lstring_to_string(U16* lstring,U64 len,char* string)
{
    for(U64 i=0;i<len;i++) {
        string[i] = (char)lstring[i];
    }
    return len;
}
U64 expand_string_to_lstring(char* string,U64 len,U16* lstring)
{
    for(U64 i=0;i<len;i++) {
        lstring[i] = string[i];
    }
    return len;
}
