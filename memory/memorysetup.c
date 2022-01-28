#include <memory/efi-memmap.h>
#include <common/kstring.h>
#include <common/linkedlist.h>
#include <memory/memorysetup.h>
#include <memory/memorytables.h>
#include <memory/gdt.h>
#include <efi/efidef.h>
#include <common/iterator.h>
void print_linked_list();
void memory_init(U64 mem_map_descriptor_size, U64 mem_map_size, U8* memory_map)
{
    CR3 CR3;
    CR0 CR0;
    CR4 CR4;
    EFER EFER;
    __asm__(
        "movq %%cr3,%0;"
        "movq %%cr4,%1;"
        "movq %%cr0,%2;"
        "movq $0xC0000080,%%rcx;"
        "rdmsr;"
        "movq %%rax, %3;"
        : "=g"(CR3.raw), "=g"(CR4.raw), "=g"(CR0.raw), "=g"(EFER.raw)
        :
        : "%rcx", "%rax", "%rdx", "memory");
    if (EFER.split.LME && CR4.split.PAE && CR0.split.PG) {
        // Start read UEFI page settings and creating new page tables
        MemMap* memmappointer = (MemMap*)memory_map;
        int mem_map_count = mem_map_size / mem_map_descriptor_size;

        //To make things easy, We decide to continue using UEFI's mapping method.
        //That is, Physical address is explicitly equivalent to its virtual address.
        //将空闲内存信息插入free_page_linked_list中。
        //听wjs的话。
        InlineFreePageNode* node;
        for (int i = 0; i < mem_map_count; i++) {
            if(memmappointer[i].Type==EfiConventionalMemory
             //||memmappointer[i].Type==EfiBootServicesCode
             //||memmappointer[i].Type==EfiBootServicesData
            ) {
                node = (void*)memmappointer[i].VirtualStart;
                node->free_mem_count = memmappointer[i].NumberOfPages;
                insert_existing_inline_node(&g_free_page_linked_list,&(node->node),-1);
            }
        }
        //合并连续信息。
        node = (InlineFreePageNode*)(g_free_page_linked_list.head.next->next);
        InlineFreePageNode* prev_node = (InlineFreePageNode*)(g_free_page_linked_list.head.next);
        while(node!=nullptr) {
            if((U64)node == prev_node->free_mem_count*PAGE_SIZE+(U64)prev_node) {
                prev_node->free_mem_count += node->free_mem_count;
                remove_inline_node(&g_free_page_linked_list,&node->node);
            }
            else {
                prev_node = node;
            }
            node = (InlineFreePageNode*)node->node.next;

        }
    }

}
void print_linked_list()
{
    InlineFreePageNode* node;
    init_iterator(InlineLinkedList,&g_free_page_linked_list_iterator,&g_free_page_linked_list);
    while(g_free_page_linked_list_iterator.next(&g_free_page_linked_list_iterator)) {
        node = (InlineFreePageNode*)g_free_page_linked_list_iterator.current;
        debug(" begins at 0x%x, 0x%x pages\n",node,node->free_mem_count);
    }
}

