#include <memory/efi-memmap.h>
#include <common/kstring.h>
#include <common/linkedlist.h>
#include <memory/memorysetup.h>
#include <memory/memorytables.h>
#include <memory/gdt.h>
//此文件不想再调试了。
// --by fhh
#undef debug
#define debug(x...)
U64 PML4E_base_address;

void memory_init(U64 mem_map_descriptor_size, U64 mem_map_size, U8* memory_map)
{
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
        : "=g"(CR3.raw), "=g"(CR4.raw), "=g"(CR0.raw), "=g"(RFLAGS.raw), "=g"(EFER.raw)
        :
        : "%rcx", "%rax", "%rdx", "memory");
    U8 four_level_paging_flag = 0;
    if (EFER.split.LME) { //check ia32-EFER.LME status
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR4.split.PAE) { //check CR4.PAE status
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR0.split.PG) { //check CR0.PG status
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (four_level_paging_flag == 3) { // check if four level paging is enabled
        PML4E_base_address = (CR3.split.base_addr)<<12;



        
        // Start read UEFI page settings and creating new page tables
        memmap* memmappointer = (memmap*)memory_map;
        memmappointer = (memmap*)memory_map;
        int mem_map_count = mem_map_size / mem_map_descriptor_size;
        for (int i = 0; i < mem_map_count - 1; i++) {
            if (memmappointer[i].VirtualStart + (memmappointer[i].NumberOfPages << 12)
                == memmappointer[i + 1].VirtualStart) {
                if (memmappointer[i].Type == 3
                    || memmappointer[i].Type == 4
                    || memmappointer[i].Type == 7) {
                    memmappointer[i].Type = 7;
                    if (memmappointer[i + 1].Type == 3
                        || memmappointer[i + 1].Type == 4
                        || memmappointer[i + 1].Type == 7) {

                        memmappointer[i].NumberOfPages += memmappointer[i + 1].NumberOfPages;
                        memmove((U8*)&memmappointer[i + 1],
                            (U8*)&memmappointer[i + 2],
                            (mem_map_count - i) * sizeof(memmap));
                        i--;
                        mem_map_count--;
                    }
                }
            }
        }

        memmappointer = (memmap*)memory_map;
        //To make things easy, We decide to continue using UEFI's mapping method.
        //That is, Physical address is explicitly equivalent to its virtual address.

    }

}
