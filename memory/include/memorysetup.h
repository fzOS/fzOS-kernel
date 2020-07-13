#ifndef MEMORY_SETUP
#define MEMORY_SETUP
#include <registers.h>
#include <graphics.h>
#include <printk.h>
#include <memory.h>
typedef struct {
    U64 beginaddr;
    U64 length;
} freemem_node;
typedef struct {
    U64 page_begin_address;
    short begin_offset;
    short remaining_bytes_in_page;
} allocated_page_list_node;
void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map,U32 color);
#endif