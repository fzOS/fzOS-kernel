#include <memory.h>
#include <types.h>
#include <linkedlist.h>
#include <memorysetup.h>
extern linked_list* freemem_linked_list;
extern linked_list* allocated_page_linked_list;
void* memalloc(U64 size)
{   
    //留出8bytes存储分配的内存大小。
    size += sizeof(U64);
    //首先，找到满足要求的地址。
    U64 addr = -1;
    linked_list_node* p = &(allocated_page_linked_list->head);
    do
    {
        p = p->next;
        if(((allocated_page_list_node*)p->data)->remaining_bytes_in_page>=size)
        {
            addr = ((allocated_page_list_node*)p->data)->begin_offset;
            *((U64*)addr) = size;
            addr+=sizeof(U64);
            ((allocated_page_list_node*)p->data)->remaining_bytes_in_page -= size;
            if(((allocated_page_list_node*)p->data)->remaining_bytes_in_page==0)
            {
                remove_node(freemem_linked_list,p);
                memfree(p->data);
                memfree(p);
            }
            break;
        }
    }
    while(p!=freemem_linked_list->tail);
    if(addr==-1)
    {
        //如果没有找到的话，去申请新的页面。
        U64 page_count = size / 4096 + 1;
        printk("Cannot get enough remaining free space.Now allocating %d pages.\n",page_count);
        addr = allocate_page(page_count);
        if(addr==-1)
            return (void*)addr;
        *((U64*)addr) = size;
        addr += sizeof(U64);
        printk("Putting remaining %d bytes into free mem list.\n",(page_count)*4096 - size);
        //将剩下的插入空闲链表里.
        allocated_page_list_node* remaining = memalloc(sizeof(allocated_page_list_node));
        remaining->begin_offset = size%4096;
        remaining->page_begin_address = (addr-sizeof(U64))+4096*(page_count-1);
        remaining->remaining_bytes_in_page = (page_count)*4096 - size;
        linked_list_node* remaining_node = memalloc(sizeof(linked_list_node));
        remaining_node->data = remaining;
        linked_list_node* current_node = &(allocated_page_linked_list->head);
        int pos = 0;
        do
        {
            current_node = current_node->next;
            pos++;
        }
        while((((allocated_page_list_node*)(current_node->data))->page_begin_address<remaining->page_begin_address)&&current_node!=freemem_linked_list->tail);
        insert_existing_node(allocated_page_linked_list,remaining_node,pos);
    }
    return (void*)addr;
}
void memfree(void* pointer)
{
    //取出分配的内存大小。
    U64 size= *(((U64*)(pointer))-1);
    printk(" Freeing space:%d\n",size);
}
U64 allocate_page(U64 page_count)
{
    U64 address = -1;
    linked_list_node* current_node = freemem_linked_list->head.next;
    do
    {
        if(((freemem_node*)current_node->data)->length>=page_count)
        {
            ((freemem_node*)current_node->data)->length -= page_count;
            address = ((freemem_node*)current_node->data)->beginaddr += page_count;
            if(!((freemem_node*)current_node->data)->length)
            {
                remove_node(freemem_linked_list,current_node);                
                memfree(current_node);
            }
            break;
        }
        current_node = current_node->next;
    }
    while(current_node!=freemem_linked_list->tail);
    return address;
}
void free_page(U64 page_begin_address,U64 page_count)
{

}