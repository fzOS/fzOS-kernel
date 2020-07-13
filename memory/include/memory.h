#ifndef MEMORY
#define MEMORY

#include <types.h>
void* memalloc(U64 size);
void memfree(void* pointer);
U64 allocate_page(U64 page_count);
void free_page(U64 page_begin_address,U64 page_count);
#endif