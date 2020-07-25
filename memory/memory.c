#include <linkedlist.h>
#include <memory.h>
#include <memorysetup.h>
#include <types.h>

extern linked_list* freemem_linked_list;
extern linked_list* allocated_page_linked_list;
extern iterator(linked_list) freemem_linked_list_iterator, allocated_page_linked_list_iterator;
void* memalloc(U64 size)
{
    printk("Attempting to allocate %d bytes.\n", size);
    init_iterator(linked_list, &allocated_page_linked_list_iterator, allocated_page_linked_list);
    //留出8bytes存储分配的内存大小。
    size += sizeof(U64);
    //首先，找到满足要求的地址。
    U64 addr = -1;
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, allocated_page_linked_list);
    while (iter.next(&iter)) {
        printk("checking %x,%d,%d\n", ((allocated_page_list_node*)(iter.current->data))->page_begin_address,
            ((allocated_page_list_node*)(iter.current->data))->begin_offset,
            ((allocated_page_list_node*)(iter.current->data))->remaining_bytes_in_page);
        if (((allocated_page_list_node*)iter.current->data)->remaining_bytes_in_page >= size) {
            addr = ((allocated_page_list_node*)(iter.current->data))->page_begin_address + ((allocated_page_list_node*)(iter.current->data))->begin_offset;
            *(U64*)addr = size;
            addr += sizeof(U64);
            ((allocated_page_list_node*)iter.current->data)->begin_offset += size;
            ((allocated_page_list_node*)iter.current->data)->remaining_bytes_in_page -= size;
            if (((allocated_page_list_node*)iter.current->data)->remaining_bytes_in_page == 0) {
                iter.remove(&iter);
            }
            break;
        }
    }
    if (addr == -1) {
        //如果没有找到的话，去申请新的页面。
        U64 page_count = size / 4096 + 1;
        printk("Cannot get enough remaining free space.Now allocating %d pages.\n",
            page_count);
        addr = allocate_page(page_count);
        if (addr == -1)
            return (void*)addr;
        *((U64*)addr) = size;
        addr += sizeof(U64);
        printk("Putting remaining %d bytes into free mem list.\n",
            (page_count)*4096 - size);
        //将剩下的插入空闲链表里.
        printk("Allocating allocated page list node.\n");
        allocated_page_list_node* remaining = memalloc(sizeof(allocated_page_list_node));
        remaining->begin_offset = size % 4096;
        remaining->page_begin_address = (addr - sizeof(U64)) + 4096 * (page_count - 1);
        remaining->remaining_bytes_in_page = (page_count)*4096 - size;
        printk("Allocating link list node.\n");
        linked_list_node* remaining_node = memalloc(sizeof(linked_list_node));
        remaining_node->data = remaining;
        linked_list_node* current_node = &(allocated_page_linked_list->head);
        int pos = -1;
        while (current_node != allocated_page_linked_list->tail && (current_node == &(allocated_page_linked_list->head) || ((allocated_page_list_node*)(current_node->data))->page_begin_address < remaining->page_begin_address)) {
            current_node = current_node->next;
            pos++;
        };
        //如果还是小，那么就是尾部了。
        if (current_node == &(allocated_page_linked_list->head) || ((allocated_page_list_node*)(current_node->data))->page_begin_address < remaining->page_begin_address) {
            pos++;
        }
        insert_existing_node(allocated_page_linked_list, remaining_node, pos);
    }
    return (void*)addr;
}
void memfree(void* pointer)
{
    //取出分配的内存大小。
    U64 size = *(((U64*)(pointer)) - 1);
    printk(" Freeing space:%d\n", size);
    U64 begin_address = (U64)(pointer - sizeof(U64));
    init_iterator(linked_list, &allocated_page_linked_list_iterator, allocated_page_linked_list);
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, allocated_page_linked_list);
    allocated_page_list_node* node;
    //搜索地址。
    while (iter.next(&iter)) {
        node = (allocated_page_list_node*)(iter.current->data);
        printk("checking %x,%d,%d\n", node->page_begin_address,
            node->begin_offset,
            node->remaining_bytes_in_page);
        if (node->page_begin_address != (begin_address & 0xFFFFFFFFFFFFF000ULL)) {
            continue;
        }
        //取出偏移。
        U16 offset = begin_address & 0xFFF;
        U8 has_next = 1;
        while (has_next && node->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL) && node->begin_offset < offset) {
            has_next = iter.next(&iter);
            node = (allocated_page_list_node*)(iter.current->data);
        }
        //如果是因为找到了位置而停下来，执行长度修改操作。
        if (!(node->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL) && node->begin_offset < offset)) {
            //无需验证是否有后面，因为当前已经在“后面”.
            //首先，如果有前面，验证是否与前面相连。
            if (iter.current->prev != &(iter.list->head)) {
                if (((allocated_page_list_node*)iter.current->prev->data)->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL)\
                &&((allocated_page_list_node*)iter.current->prev->data)->begin_offset + ((allocated_page_list_node*)iter.current->prev->data)->remaining_bytes_in_page == )
            }
        }
    }
}
U64 allocate_page(U64 page_count)
{
    U64 address = -1;
    linked_list_node* current_node = freemem_linked_list->head.next;
    do {
        if (((freemem_node*)current_node->data)->length >= page_count) {
            ((freemem_node*)current_node->data)->length -= page_count;
            address = ((freemem_node*)current_node->data)->beginaddr += page_count;
            if (!((freemem_node*)current_node->data)->length) {
                remove_node(freemem_linked_list, current_node);
                memfree(current_node);
            }
            break;
        }
        current_node = current_node->next;
    } while (current_node != freemem_linked_list->tail);
    return address;
}
void free_page(U64 page_begin_address, U64 page_count)
{
    //找到地址。
    linked_list_node* node = &(freemem_linked_list->head);
    do {
        node = node->next;
    } while (((freemem_node*)node->data)->beginaddr < page_begin_address && node != freemem_linked_list->tail);
    //又是老套的查询……
    if (((freemem_node*)node->data)->beginaddr < page_begin_address) {
        //尾插。
    } else {
        //后面相连，加后面.
        if ((page_begin_address + (page_count << 12)) == ((freemem_node*)node->data)->beginaddr) {
            ((freemem_node*)node->data)->beginaddr -= (page_count << 12);
            ((freemem_node*)node->data)->length += page_count;
            //前后都相连，删除一个结点。
            if (node->prev != &(freemem_linked_list->head) && ((freemem_node*)node->prev->data)->beginaddr + (((freemem_node*)node->prev->data)->length << 12) == page_begin_address) {
                ((freemem_node*)node->data)->beginaddr -= ((freemem_node*)node->prev->data)->length;
                ((freemem_node*)node->data)->length += ((freemem_node*)node->prev->data)->length;
                node = node->prev;
                remove_node(freemem_linked_list, node);
                memfree(node->data);
                memfree(node);
            }
            return;
        } else {
            node = node->prev;
            //前面相连，加前面。
            if (node != &(freemem_linked_list->head) && ((freemem_node*)node->data)->beginaddr + ((freemem_node*)node->data)->length == page_begin_address) {
                ((freemem_node*)node->data)->length += page_count;
                return;
            }
            //前后都不相连，添加结点。
            node = node->next;
            freemem_node* free_node = memalloc(sizeof(freemem_node));
            free_node->beginaddr = page_begin_address;
            free_node->length = page_count;
            linked_list_node* list_node = memalloc(sizeof(linked_list_node));
            list_node->data = free_node;
            insert_existing_node_before_existing(freemem_linked_list, list_node, node);
        }
    }
}