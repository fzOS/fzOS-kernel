#include <memorysetup.h>
#include <efi-memmap.h>
#include <kstring.h>
#include <memorytables.h>
#include <linkedlist.h>
linked_list* freemem_linked_list;
linked_list* allocated_page_linked_list;
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
    if (EFER.split.LME){//check ia32-EFER.LME status
        long_mode_flag = 1;
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR4.split.PAE){//check CR4.PAE status
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (CR0.split.PG){//check CR0.PG status
        four_level_paging_flag = four_level_paging_flag + 1;
    }
    if (four_level_paging_flag == 3){// check if four level paging is enabled
        // Start read UEFI page settings and creating new page tables
        memmap* memmappointer = (memmap*)memory_map;
        memmappointer = (memmap*)memory_map;
        int mem_map_count = mem_map_size/mem_map_descriptor_size;
        for(int i=0;i<mem_map_count-1;i++)
        {
            if(memmappointer[i].PhysicalStart+(memmappointer[i].NumberOfPages<<12)
            ==memmappointer[i+1].PhysicalStart)
            {
                if(memmappointer[i].Type==3
                 ||memmappointer[i].Type==4
                 ||memmappointer[i].Type==7)
                {
                    memmappointer[i].Type = 7;
                    if(memmappointer[i+1].Type==3
                     ||memmappointer[i+1].Type==4
                     ||memmappointer[i+1].Type==7)
                    {

                        memmappointer[i].NumberOfPages+=memmappointer[i+1].NumberOfPages;
                        memmove((U8*)&memmappointer[i+1],
                                (U8*)&memmappointer[i+2],
                                (mem_map_count-i)*sizeof(memmap));
                        i--;
                        mem_map_count--;
                    }
                }
            }
        }
        U64 free_entry=0;
        memmappointer = (memmap*)memory_map;
        for (U8 i = 0; i < (mem_map_count); i++){
            if(memmappointer->Type==7)
            {
                free_entry++;
            }
            memmappointer++;
        }

        memmappointer = (memmap*)memory_map;
        #if 0
        memory_address maxaddr;
        maxaddr.raw = (memmappointer[mem_map_count-1]).PhysicalStart
                   +(((memmappointer[mem_map_count-2]).NumberOfPages)<<12)-1;
        printk("Our maximum Memory Address 0x%x belongs PML4E#%x,"
               "PDPE#%x,PDE#%x,PTE#%x,offset+%x.\n",
                maxaddr.raw,
                maxaddr.split.page_map_level_4_offset,
                maxaddr.split.page_directory_pointer_offset,
                maxaddr.split.page_directory_offset,
                maxaddr.split.page_table_offset,
                maxaddr.split.physical_page_offset
        );
        #endif
        //Now, we can calculate how much pages do we need.
        //To make things easy, We decide to continue using UEFI's mapping method.
        //That is, Physical address is explicitly equivalent to its virtual address.
        
        U64 total_required_size = sizeof(linked_list)*2
                                  +(free_entry)
                                  *(sizeof(linked_list_node)+sizeof(freemem_node))
                                  +sizeof(linked_list_node)+sizeof(allocated_page_list_node);
        //find the proper space to hold.
        for (U8 i = 0; i < (mem_map_count); i++){
            if(memmappointer[i].Type==7
             &&memmappointer[i].NumberOfPages>(total_required_size>>12)+1)
            {
                void* current_pointer = (linked_list*)memmappointer[i].PhysicalStart;
                //alter free page information.
                memmappointer[i].NumberOfPages -= (total_required_size>>12)+1;
                memmappointer[i].PhysicalStart += ((total_required_size&0xFFFFFFFFFFFFF000ULL)+0x1000);
                //Construct the main list first.
                freemem_linked_list = current_pointer;
                freemem_linked_list->tail = &(freemem_linked_list->head);
                current_pointer += sizeof(linked_list);
                //Then,Let's construct freemem_node values.
                freemem_node* freemem_nodes = current_pointer;
                for(U8 i = 0; i < (mem_map_count); i++)
                {
                    if(memmappointer[i].Type != 7)
                    {
                        continue;
                    }
                    ((freemem_node*)current_pointer)->beginaddr = memmappointer[i].PhysicalStart;
                    ((freemem_node*)current_pointer)->length    = memmappointer[i].NumberOfPages;
                    current_pointer += sizeof(freemem_node);
                }
                //Next,Let's finish linkage process.
                linked_list_node* current_node = &(freemem_linked_list->head);
                for(U8 i = 0; i < (free_entry); i++)
                {
                    current_node = (linked_list_node*)current_pointer;
                    current_node->data = (void*)(freemem_nodes+i);
                    insert_existing_node(freemem_linked_list,current_node,-1);
                    current_pointer += sizeof(linked_list_node);
                }
                //Hopefully it works....
                current_node = freemem_linked_list->head.next;
                //And finally,Let's construct free memory link list in allocated pages.
                allocated_page_linked_list = current_pointer;
                allocated_page_linked_list->tail = &(allocated_page_linked_list->head);
                current_pointer += sizeof(linked_list);
                allocated_page_list_node* node = current_pointer;
                node->page_begin_address = memmappointer[i].PhysicalStart 
                                          -((total_required_size&0xFFFFFFFFFFFFF000ULL)+0x1000);
                node->begin_offset = total_required_size;
                node->remaining_bytes_in_page = 4096 - total_required_size;
                current_pointer += sizeof(allocated_page_list_node);
                ((linked_list_node*)current_pointer)->data=node;
                insert_existing_node(allocated_page_linked_list,current_pointer,-1);
                printk(" Memory: free-space initialization done.\n");                
                break;
            }
        }
        printk("Testing memalloc.\n");
        U64* test = memalloc(2333);
        printk("Successfully got %d bytes from addr %x.\n",*(test-1),(U64)test);
        U64* test2 = memalloc(23333);
        printk("Successfully got %d bytes from addr %x.\n",*(test2-1),(U64)test2);
        #if 0
        //Let's check if we can handle paging properly.
        int test=0xdeadbeef;
        memory_address testaddr;
        testaddr.raw = (U64)&test;
        printk("Memory Address 0x%x belongs PML4E#%x,PDPE#%x,PDE#%x,PTE#%x,offset+%x.\n",
                testaddr.raw,
                testaddr.split.page_map_level_4_offset,
                testaddr.split.page_directory_pointer_offset,
                testaddr.split.page_directory_offset,
                testaddr.split.page_table_offset,
                testaddr.split.physical_page_offset
        );
        printk("Target PDPE is at%x.\n",*((U64*)(U64)CR3.split.base_addr));
        #endif
    }

    //To let the compiler KNOW we use the variable.
    //Should be removed after functions being added.
    long_mode_flag += 0;
}