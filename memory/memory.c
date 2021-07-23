#include <common/linkedlist.h>
#include <memory/memory.h>
#include <memory/memorysetup.h>
#include <types.h>

//此文件不想再调试了。
// --by fhh
//#undef debug
//#define debug(x...)

inline_linked_list free_page_linked_list = {
    .tail = &(free_page_linked_list.head)
};

void* memalloc(U64 size)
{
    return NULL;
}

void memfree(void* pointer)
{
    return;
}

void* allocate_page(int count)
{
    return NULL;
}

void free_page(void* page_address,int count)
{

}
int memcpy(void* dest,void* src,int n)
{
    for(int i=0;i<n;i++)
    {
        ((U8*)dest)[i] = ((U8*)src)[i];
    }
    return n;
}
int memcmp(void* first,void* second,int n)
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
        return memcpy(dest,src,n);
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

//END
