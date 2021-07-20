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
    inline_linked_list_node node;
    U64 memory_pool_size;
    U64 entries[(MAXIMUM_POOL_SIZE-sizeof(inline_linked_list_node)-sizeof(U64))/sizeof(U64)];
} memory_pool_node;
#define ENTRIES_IN_A_PAGE ((MAXIMUM_POOL_SIZE-sizeof(inline_linked_list_node)-sizeof(U64))/sizeof(U64))


extern enum MEM_POOL_SIZE {
    MEM_POOL_32,
    MEM_POOL_64,
    MEM_POOL_128,
    MEM_POOL_256,
    MEM_POOL_512,
    MEM_POOL_1024,
    MEM_POOL_2048
} MEM_POOL_SIZE;
extern inline_linked_list mem_pool[7];


void* memalloc(U64 size);
void memfree(void* pointer);
U64 memset(void* pointer,byte value,int n);
U64 allocate_page(U64 page_count);
void free_page(U64 page_begin_address,U64 page_count);
void print_partial_memory(void);
void print_free_page(void);
#endif
