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
int sprintk(char* buf,char* format,...)
{
    int count=0;
    va_list arg;
    va_start(arg, format);
    char* bufp = buf;
    char* pointer = format;
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
                case 'x':{printk("sprintk:Stub!\n");break;}
                case 'b':{printk("sprintk:Stub!\n");break;}
                case 'w':{printk("sprintk:Stub!\n");break;}
                case 's':{printk("sprintk:Stub!\n");break;}
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
