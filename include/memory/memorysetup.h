#ifndef MEMORY_SETUP
#define MEMORY_SETUP
#include <common/registers.h>
#include <drivers/graphics.h>
#include <common/printk.h>
#include <memory/memory.h>
void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map);
extern U64 PML4E_base_address;
#endif
