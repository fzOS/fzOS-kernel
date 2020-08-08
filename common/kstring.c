 #include <kstring.h>
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
int memcopy(void* dest,void* src,int n)
{
    for(int i=0;i<n;i++)
    {
        ((U8*)dest)[i] = ((U8*)src)[i];
    }
    return n;
}
int memcomp(void* first,void* second,int n)
{
    U8 result=0;
    while(n&&!result)
    {
        result+=(*(U8*)first-*(U8*)second);
        (U8*)first++;
        (U8*)second++;
        n--;
    }
    return result;
}
int memmove(void* dest,void* src,int n)
{
    if(dest<src)
    {
        return memcopy(dest,src,n);
    }
    else //存在覆盖问题，从尾至头搬运。
    {
        for(int i=n-1;i>0;i--)
        {
            ((U8*)dest)[i] = ((U8*)src)[i];
        }
        return n;
    }
}
#pragma GCC optimize 1
U64 memset(void* pointer,U8 value,int n)
{
    for (int i = 0; i < n; i++) ((U8*)pointer)[i] = value;
    return n;
}