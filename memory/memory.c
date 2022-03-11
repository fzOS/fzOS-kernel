#include <common/linkedlist.h>
#include <memory/memory.h>
#include <memory/memorysetup.h>
#include <types.h>


InlineLinkedList g_free_page_linked_list = {
    .tail = &(g_free_page_linked_list.head)
};
Iterator(InlineLinkedList) g_free_page_linked_list_iterator;
void* memalloc(U64 size)
{
    //奢侈一把！
    U64 page_count = (size+sizeof(U64)-1)/PAGE_SIZE+1;
    void* p = allocate_page(page_count);
    *(U64*)p = page_count;
    return p+sizeof(U64);
}

void* memrealloc(void* pointer, U64 new_size)
{
    void* p = memalloc(new_size);
    U64 old_page_count = *(U64*)(pointer-sizeof(U64));
    memcpy(p,pointer,old_page_count*PAGE_SIZE-sizeof(U64));
    memfree(pointer);
    return p;
}

void memfree(void* pointer)
{
    U64 page_count = *(U64*)(pointer-sizeof(U64));
    free_page(pointer-sizeof(U64),page_count);
    return;
}

void* allocate_page(int count)
{
    void* p = nullptr;
    if(count<=0) {
        return p;
    }
    //倒着分配。
    InlineFreePageNode* node = (InlineFreePageNode*)g_free_page_linked_list.tail;
    while(node!=(InlineFreePageNode*)&g_free_page_linked_list.head) {
        if(node->free_mem_count>=count) {
            U64 top = (U64)node + (node->free_mem_count)*PAGE_SIZE - count*PAGE_SIZE;
            node->free_mem_count -= count;
            p = (void*) top;
            if(!node->free_mem_count) {
                remove_inline_node(&g_free_page_linked_list,&node->node);
            }
            return p;
        }
        node = (InlineFreePageNode*)(node->node.prev);
    }
    return p;
}

void free_page(void* page_address,int count)
{
    if(count<=0) {
        return;
    }
    InlineFreePageNode* node = (InlineFreePageNode*)(g_free_page_linked_list.tail);
    InlineFreePageNode* prev_node = (InlineFreePageNode*)(g_free_page_linked_list.tail);
    while(prev_node!=(InlineFreePageNode*)&(g_free_page_linked_list.head)
       &&(void*)prev_node>page_address) {
        node = prev_node;
        prev_node = (InlineFreePageNode*)(prev_node->node.prev);
    }


    if((prev_node!=(InlineFreePageNode*)&(g_free_page_linked_list.head))
     &&(U64)(prev_node->free_mem_count)*PAGE_SIZE + (U64)prev_node == (U64)page_address) {
        //拼到前面去
        prev_node->free_mem_count += count;
        //如果和后面也相连，合并。
        if((prev_node!=node)&&(page_address+count*PAGE_SIZE == node)) {
            prev_node->free_mem_count += node->free_mem_count;
            remove_inline_node(&g_free_page_linked_list,(InlineLinkedListNode*)node);
        }
    }
    else {
        InlineFreePageNode* new_node = page_address;
        //和后面相连，合并。
        if((prev_node!=node)&&(page_address+count*PAGE_SIZE == node)) {
            prev_node->node.next = (InlineLinkedListNode*)new_node;
            node->node.next->prev = (InlineLinkedListNode*)new_node;
            new_node->free_mem_count = node->free_mem_count + count;
            new_node->node.prev = (InlineLinkedListNode*)prev_node;
            new_node->node.next = node->node.next;
        }
        else {
            //都不相连，创建新节点。
            new_node->free_mem_count = count;
            if(prev_node!=node) {
                insert_existing_inline_node_before_existing(&g_free_page_linked_list,(InlineLinkedListNode*)new_node,(InlineLinkedListNode*)node);
            }
            else {
                insert_existing_inline_node(&g_free_page_linked_list,(InlineLinkedListNode*)new_node,-1);
            }
//             new_node->node.prev = (inline_linked_list_node*)prev_node;
//             prev_node->node.next = (inline_linked_list_node*)new_node;
//             node->node.prev = (inline_linked_list_node*)new_node;
//             new_node->node.next = (inline_linked_list_node*) node;
        }
        if(new_node->node.next==nullptr) {
            g_free_page_linked_list.tail = (InlineLinkedListNode*)new_node;
        }
    }
}
int memcpy(void* dest,const void* src,U64 n)
{
    for(int i=0;i<n;i++)
    {
        ((U8*)dest)[i] = ((U8*)src)[i];
    }
    return n;
}
int memcmp(const void* first,const void* second,U64 n)
{
    U8 result=0;
    while(n&&!result)
    {
        result+=(*(U8*)first-*(U8*)second);
        first++;
        second++;
        n--;
    }
    return result;
}
int memmove(void* dest,void* src,U64 n)
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
#ifndef __clang__
#pragma GCC optimize 1
#endif
U64 memset(void* pointer, U8 value, U64 n)
{
    for (int i = 0; i < n; i++) ((U8*)pointer)[i] = value;
    return n;
}

//END
