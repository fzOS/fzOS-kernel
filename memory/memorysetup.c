#include <memorysetup.h>

void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map,U32 color){
    /*U64 *memmappointer;
    memmappointer = (U64 *) memory_map;
    printk("\n Memory Map: ", color);
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
    //printk("\n RFLAGS value:", color);
    //kernel_print_U64_hex(RFLAGValue, color);
    U8 four_level_paging_flag = 0;
    U8 ia32e_mode_flag = 0;
    printk("\n EFER value:%x",EFERValue);
    printk("\n CR0 value:%x",CR0Value);
    printk("\n CR3 value:%x",CR3Value);
    PageDictionAddr = (CR3Value&0xffffffffffff000) >> 12;
    printk("\n Page Diction Address Base:%x",PageDictionAddr);
    printk("\n CR4 value:%x",CR4Value);
    if (((EFERValue >> 8) & 0x1) == 0x1){//check ia32-EFER.LME status
        ia32e_mode_flag = 1;
        printk("\n CPU is at IA-32E mode");
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (((CR4Value >> 5) & 0x1) == 0x1){//check CR4.PAE status
        printk("\n CR4.PAE is enabled", color);
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (((CR0Value >> 31) & 0x1) == 0x1){//check CR0.PG status
        printk("\n CR0.PG is enabled", color);
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (four_level_paging_flag == 3){// check if four level paging is enabled
        printk("\n 4-Level Paging is enabled; Attempt to handle it now.\n",color);
        // Start read UEFI page settings and creating new page tables
    }

    //To let the compiler KNOW we use the variable.
    //Should be removed after functions being added.
    ia32e_mode_flag += 0;
}