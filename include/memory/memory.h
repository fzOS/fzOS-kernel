#ifndef MEMORY
#define MEMORY

#include <types.h>
#include <common/linkedlist.h>
//For memory_ng.
//This defines different memory types.
/*
    使用内存池算法实现内存的分配与回收。
    由于AMD64平台上的char*长度是8字节，此处最小的内存空闲块从32字节开始。
 */
#define MINIMUM_POOL_SIZE 32
/*
    大于一页（4KB）的分配没有意义。
    因此，我们直接将大于4K的转换成直接申请页面。
 */
#define MAXIMUM_POOL_SIZE PAGE_SIZE
//偷懒：把“是否已经分配”的标记放在最高位（bit 63），减少开销。
#define ALLOCATED_MARKER  0x8000000000000000
typedef struct {
    InlineLinkedListNode node;
    U64 memory_pool_size;
    U64 entries[(MAXIMUM_POOL_SIZE-sizeof(InlineLinkedListNode)-sizeof(U64))/sizeof(U64)];
} MemoryPoolNode;
#define ENTRIES_IN_A_PAGE ((MAXIMUM_POOL_SIZE-sizeof(inline_linked_list_node)-sizeof(U64))/sizeof(U64))

//听wjs的，空闲链表信息直接保存在空闲区块里。
typedef struct {
    InlineLinkedListNode node;
    U64 free_mem_count;
} InlineFreePageNode;

void* memalloc(U64 size);
void memfree(void* pointer);
int memcpy(void* dest,void* src,U64 n);
int memcmp(const void* first,const void* second,U64 n);
int memmove(void* dest,void* src,U64 n);
U64 memset(void* pointer,U8 value,U64 n);

void* allocate_page(int count);
void free_page(void* page_address,int count);
void* memalloc(U64 size);
void memfree(void* pointer);
void* memrealloc(void* pointer, U64 new_size);
extern InlineLinkedList g_free_page_linked_list;
extern Iterator(InlineLinkedList) g_free_page_linked_list_iterator;
#endif
