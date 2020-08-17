#ifndef MEMORY
#define MEMORY

#include <types.h>
void* memalloc(U64 size);
void memfree(void* pointer);
U64 memset(void* pointer,byte value,int n);
U64 allocate_page(U64 page_count);
void free_page(U64 page_begin_address,U64 page_count);
void print_partial_memory(void);
void print_free_page(void);
#endif