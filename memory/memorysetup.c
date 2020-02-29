#include <graphic.h>
#include <kerneldisplay.h>
#include <memorysetup.h>
#include <types.h>

void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map,U32 color){
    /*U64 *memmappointer;
    memmappointer = (U64 *) memory_map;
    kernel_log_print_string("\n Memory Map: ", color);
    for (U8 i = 0; i < (mem_map_size / mem_map_descriptor_size); i++){
        kernel_print_U64_hex(*memmappointer, color);
        memmappointer = memmappointer + 1;
        kernel_print_U64_hex(*memmappointer, color);
        memmappointer = memmappointer + 1;
        kernel_print_U64_hex(*memmappointer, color);
        memmappointer = memmappointer + 1;
        kernel_print_U64_hex(*memmappointer, color);
        memmappointer = memmappointer + 1;
        kernel_print_U64_hex(*memmappointer, color);
        memmappointer = memmappointer + 2;
    }*/
    U64 CR3Value;
    __asm__ __violate__(
        "movl %%cr3,%0"
        : "g"(CR3Value)
        : 
        : "memory");
    kernel_log_print_string("\n CR3 value", color);
    kernel_print_U64_hex(CR3Value, color);
}