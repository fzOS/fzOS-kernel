#include <memorysetup.h>

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
    U64 CR0Value,CR3Value,CR4Value,PageDictionAddr,RFLAGValue,EFERValue;
    __asm__(
        "movq %%cr3,%0;"
        "movq %%cr4,%1;"
        "movq %%cr0,%2;"
        //"movq %%rflags,%3;"
        "movq $0xC0000080,%%rcx;"
        "rdmsr;"
        "movq %%rax, %4;"
        : "=g"(CR3Value), "=g"(CR4Value), "=g"(CR0Value), "=g"(RFLAGValue), "=g"(EFERValue)
        : 
        : "%rcx","%rax","%rdx","memory");
    //kernel_log_print_string("\n RFLAGS value:", color);
    //kernel_print_U64_hex(RFLAGValue, color);
    kernel_log_print_string("\n EFER value:", color);
    kernel_print_U64_hex(EFERValue, color);
    kernel_log_print_string("\n CR0 value:", color);
    kernel_print_U64_hex(CR0Value, color);
    kernel_log_print_string("\n CR3 value:", color);
    kernel_print_U64_hex(CR3Value, color);
    PageDictionAddr = (CR3Value&0xffffffffffff000) >> 12;
    kernel_log_print_string("\n Page Diction Address Base:", color);
    kernel_print_U64_hex(PageDictionAddr, color);
    kernel_log_print_string("\n CR4 value:", color);
    kernel_print_U64_hex(CR4Value, color);
}