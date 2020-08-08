#include <linkedlist.h>
#include <memory.h>
#include <memorysetup.h>
#include <types.h>

//此文件不想再调试了。
// --by fhh
#undef debug
#define debug(x...)

extern linked_list* freemem_linked_list;
extern linked_list* allocated_page_linked_list;
extern iterator(linked_list) freemem_linked_list_iterator, allocated_page_linked_list_iterator;
void* memalloc(U64 size)
{
    //由于一些原因（确保不会出现小于64bit的碎片），我们必须将分配size按照8byte对齐。
    size = ((size-1)&0xFFFFFFFFFFFFFFF8ULL)+0x8;
    debug("allocating %d bytes of memory.\n", size);
    init_iterator(linked_list, &allocated_page_linked_list_iterator, allocated_page_linked_list);
    //留出8bytes存储分配的内存大小。
    size += sizeof(U64);
    //首先，找到满足要求的地址。
    U64 addr = -1;
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, allocated_page_linked_list);
    while (iter.next(&iter)) {
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
        addr = allocate_page(page_count);
        if (addr == -1)
            return (void*)addr;
        *((U64*)addr) = size;
        addr += sizeof(U64);
        //将剩下的插入空闲链表里.
        allocated_page_list_node* remaining = memalloc(sizeof(allocated_page_list_node));
        remaining->begin_offset = size % 4096;
        remaining->page_begin_address = (addr - sizeof(U64)) + 4096 * (page_count - 1);
        remaining->remaining_bytes_in_page = (page_count)*4096 - size;
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
    U64 begin_address = (U64)(pointer - sizeof(U64));
    if(!size)
        return;
    //若需要释放的不止一个block，则尽可能多地释放block后，释放剩下的内存。
    if(size>4096) {
        U64 page_begin_address_in_range = ((begin_address-1) & 0xFFFFFFFFFFFFF000ULL)+0x1000;
        if((begin_address+size-page_begin_address_in_range)>=4096) {
            free_page(page_begin_address_in_range,(begin_address+size-page_begin_address_in_range)>>12);
            //归还前面的部分。
            *(((U64*)(pointer)) - 1) = page_begin_address_in_range-begin_address;
            memfree(pointer);
            //归还后面的部分。
            size = (begin_address+size)&0xFFF;
            begin_address = (begin_address+size)&0xFFFFFFFFFFFFF000ULL;
        }
    }
    debug("Freeing %d bytes from %x.\n", size, begin_address);
    init_iterator(linked_list, &allocated_page_linked_list_iterator, allocated_page_linked_list);
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, allocated_page_linked_list);
    allocated_page_list_node* node = iter.current->data;
    //搜索地址。
    while (iter.next(&iter) && (node = iter.current->data, node->page_begin_address <= (begin_address & 0xFFFFFFFFFFFFF000ULL))) {        node = (allocated_page_list_node*)(iter.current->data);
        //取出偏移。
        U16 offset = begin_address & 0xFFF;
        U8 has_next = 1;
        while (has_next && node->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL) && node->begin_offset < offset) {
            has_next = iter.next(&iter);
            node = (allocated_page_list_node*)(iter.current->data);
        }
        //如果是因为找到了位置而停下来，执行长度修改操作。
        if (node->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL) && node->begin_offset > offset) {
            debug("Found correct position:%x\n", node->page_begin_address);
            //无需验证是否有后面，因为当前已经在“后面”.
            //首先，如果有前面，验证是否与前面相连。
            if ((iter.current->prev != &(iter.list->head)) && ((allocated_page_list_node*)iter.current->prev->data)->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL)
                && ((allocated_page_list_node*)iter.current->prev->data)->begin_offset + ((allocated_page_list_node*)iter.current->prev->data)->remaining_bytes_in_page == begin_address) {
                ((allocated_page_list_node*)iter.current->prev->data)->remaining_bytes_in_page += size;
                //如果与后面也相连，删除后面的结点。
                if (begin_address + size == (node->begin_offset | node->page_begin_address)) {
                    ((allocated_page_list_node*)iter.current->prev->data)->remaining_bytes_in_page += node->remaining_bytes_in_page;
                    iter.remove(&iter);
                }
                goto check_delete;
            } else {
                //Again,判断后面是否相连。
                if ((node->page_begin_address == (begin_address & 0xFFFFFFFFFFFFF000ULL) && begin_address + size == (node->begin_offset | node->page_begin_address))) {
                    node->begin_offset -= size;
                    node->remaining_bytes_in_page += size;
                    goto check_delete;
                }
                //最后，如果都不相连，新增结点，并插入队列，
                allocated_page_list_node* remaining = memalloc(sizeof(allocated_page_list_node));
                remaining->begin_offset = begin_address & 0x0FFFULL;
                remaining->page_begin_address = begin_address & 0xFFFFFFFFFFFFF000ULL;
                remaining->remaining_bytes_in_page = size;
                linked_list_node* remaining_node = memalloc(sizeof(linked_list_node));
                remaining_node->data = remaining;
                debug("inserting at %x,existing is %x.\n",remaining->page_begin_address,((allocated_page_list_node*)iter.current->data)->page_begin_address);
                insert_existing_node_before_existing(allocated_page_linked_list, remaining_node, iter.current);
                //由于新添加的不可能大于1个page，因此直接return。
                return;
            }
            debug("Failed to free memory at addr %x.\n", begin_address);
            break;
        }
    }
    debug("Cannot find required address.\n");
    allocated_page_list_node* remaining = memalloc(sizeof(allocated_page_list_node));
    remaining->begin_offset = begin_address & 0x0FFFULL;
    remaining->page_begin_address = begin_address & 0xFFFFFFFFFFFFF000ULL;
    remaining->remaining_bytes_in_page = size;
    linked_list_node* remaining_node = memalloc(sizeof(linked_list_node));
    remaining_node->data = remaining;
    debug("inserting at %x,existing is %x.\n",remaining->page_begin_address,((allocated_page_list_node*)iter.current->data)->page_begin_address);
    insert_existing_node_before_existing(allocated_page_linked_list, remaining_node, iter.current);
    check_delete:
    if(!(node->remaining_bytes_in_page%4096)) { //满了，删！
        free_page(node->page_begin_address,node->remaining_bytes_in_page%4096);
        iter.remove(&iter);
    }
}
U64 allocate_page(U64 page_count)
{
    U64 address = -1;
    linked_list_node* current_node = freemem_linked_list->head.next;
    do {
        if (((freemem_node*)current_node->data)->length >= page_count) {
            ((freemem_node*)current_node->data)->length -= page_count;
            address = ((freemem_node*)current_node->data)->beginaddr;
            ((freemem_node*)current_node->data)->beginaddr += (page_count << 12);
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

//DEBUG,显示当前空闲内存的分配情况。
void print_partial_memory(void)
{
    debug("Partial memory:\n");
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, allocated_page_linked_list);
    while (iter.next(&iter)) {
        debug("%x,%d\n", ((allocated_page_list_node*)(iter.current->data))->page_begin_address | ((allocated_page_list_node*)(iter.current->data))->begin_offset,
            ((allocated_page_list_node*)(iter.current->data))->remaining_bytes_in_page);
    }
}
void print_free_page(void)
{
    debug("Free page:\n");
    iterator(linked_list) iter;
    init_iterator(linked_list, &iter, freemem_linked_list);
    while (iter.next(&iter)) {
        debug("%x,%d\n", ((freemem_node*)(iter.current->data))->beginaddr,
            ((freemem_node*)(iter.current->data))->length);
    }
}
//END