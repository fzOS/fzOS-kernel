#ifndef MEMORY_SETUP
#define MEMORY_SETUP
#include <registers.h>
#include <graphics.h>
#include <printk.h>

void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map,U32 color);
#endif