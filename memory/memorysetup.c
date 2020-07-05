#include <memorysetup.h>
#include <efi-memmap.h>
#include <kstring.h>
void memory_init(U64 mem_map_descriptor_size,U64 mem_map_size,U8 *memory_map,U32 color) {
    CR3 CR3;
    CR0 CR0;
    CR4 CR4;
    EFER EFER;
    RFLAGS RFLAGS;
    __asm__(
        "movq %%cr3,%0;"
        "movq %%cr4,%1;"
        "movq %%cr0,%2;"
        //"movq %%rflags,%3;"
        "movq $0xC0000080,%%rcx;"
        "rdmsr;"
        "movq %%rax, %4;"
        : "=g"(CR3), "=g"(CR4), "=g"(CR0), "=g"(RFLAGS), "=g"(EFER)
        : 
        : "%rcx","%rax","%rdx","memory");
    //printk("\n RFLAGS value:", color);
    //kernel_print_U64_hex(RFLAGValue, color);
    U8 four_level_paging_flag = 0;
    U8 long_mode_flag = 0;
    printk("\n EFER value:%x",EFER.raw);
    printk("\n CR0 value:%x",CR0.raw);
    printk("\n CR3 value:%x",CR3.raw);
    printk("\n Page Diction Address Base:%x",CR3.split.base_addr);
    printk("\n CR4 value:%x",CR4.raw);
    if (EFER.split.LME){//check ia32-EFER.LME status
        long_mode_flag = 1;
        printk("\n CPU is at long mode");
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR4.split.PAE){//check CR4.PAE status
        printk("\n CR4.PAE is enabled", color);
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR0.split.PG){//check CR0.PG status
        printk("\n CR0.PG is enabled", color);
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (four_level_paging_flag == 3){// check if four level paging is enabled
        printk("\n 4-Level Paging is enabled; Attempt to handle it now.\n",color);
        // Start read UEFI page settings and creating new page tables
        memmap* memmappointer = (memmap*)memory_map;
        memmappointer = (memmap*)memory_map;
        int mem_map_count = mem_map_size/mem_map_descriptor_size;
        for(int i=0;i<mem_map_count-1;i++)
        {
            if(memmappointer[i].PhysicalStart+(memmappointer[i].NumberOfPages<<12)==memmappointer[i+1].PhysicalStart)
            {
                if(memmappointer[i].Type==3||memmappointer[i].Type==4||memmappointer[i].Type==7)
                {
                    memmappointer[i].Type = 7;
                    if(memmappointer[i+1].Type==3||memmappointer[i+1].Type==4||memmappointer[i+1].Type==7)
                    {

                        memmappointer[i].NumberOfPages+=memmappointer[i+1].NumberOfPages;
                        memmove((U8*)&memmappointer[i+1],(U8*)&memmappointer[i+2],(mem_map_count-i)*sizeof(memmap));
                        i--;
                        mem_map_count--;
                    }
                }
            }
        }
        memmappointer = (memmap*)memory_map;
        #if 0
        printk("\n Memory Map: \n", color);
        printk(" Type PhysStart VirtStart PageCount Attr\n");
        for (U8 i = 0; i < (mem_map_count); i++){
            printk(" %s %x %x %x %x\n", mem_types[memmappointer->Type],
                                        memmappointer->PhysicalStart,
                                        memmappointer->VirtualStart,
                                        memmappointer->NumberOfPages,
                                        memmappointer->Attribute);
            memmappointer++;
        }
        #endif
        //Now, we can calculate how much pages do we need.

    }

    //To let the compiler KNOW we use the variable.
    //Should be removed after functions being added.
    long_mode_flag += 0;
}