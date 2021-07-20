#include <common/linkedlist.h>
#include <memory/memory.h>
typedef iterator(linked_list) linked_list_iterator;
int iterator_linked_list_next(linked_list_iterator* this)
{
    if(this->current==this->list->tail) {
        return 0;
    }
    this->current = this->current->next;
    return 1;
}
int iterator_linked_list_prev(linked_list_iterator* this)
{
    if(this->current==this->list->head.next) {
        return 0;
    }
    this->current = this->current->prev;
    return 1;
}
void iterator_linked_list_remove(linked_list_iterator* this)
{
    //首先，判断我们是不是在头结点。
    if(this->current!=&(this->list->head)) {
        //如果当前不是尾部的话，修改一下下一个结点的信息。
        if(this->current!=(this->list->tail)) {
            this->current->next->prev = this->current->prev;
        }
        //如果是尾部的话，修改tail的记录信息。
        else {
            this->list->tail = this->current->prev;
        }
        this->current->prev->next = this->current->next;
        //向前一位。
        linked_list_node* p = this->current;
        this->current = this->current->prev;
        //回收内存。
        memfree(p->data);
        memfree(p);
    }
    else {
        return;
    }
}
void insert_existing_node(linked_list* list,linked_list_node* node,int pos)
{
    //如果位置<0,那么直接在尾部插入。
    if(pos<0)
    {
        node->next = nullptr;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        return;
    }
    //如果位置>0,则找到位置后插入。
    int i=0;
    linked_list_node* p = &(list->head);
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    //在其后方插入。
    node->next = p->next;
    node->prev = p;
    p->next = node;
    //如果本身p就是tail的话，将tail后移。
    if(list->tail==p)
    {
        list->tail = list->tail->next;
    }
    return;
}
void insert_existing_node_before_existing(linked_list* list,linked_list_node* node,linked_list_node* existing)
{
    //插入。
    node->prev = existing->prev;
    node->next = existing;
    if(existing != &(list->head)) {
        existing->prev->next = node;
        existing->prev = node;
    }
        
}
void remove_node_pos(linked_list* list,int pos)
{
    int i=0;
    linked_list_node* p = list->head.next;
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    remove_node(list,p);
}
void remove_node(linked_list* list,linked_list_node* node)
{
    if(list->tail == node)
    {
        node->prev->next = nullptr;
        list->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
}

void insert_existing_inline_node(inline_linked_list* list,inline_linked_list_node* node,int pos)
{
    //如果位置<0,那么直接在尾部插入。
    if(pos<0)
    {
        node->next = nullptr;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        return;
    }
    //如果位置>0,则找到位置后插入。
    int i=0;
    inline_linked_list_node* p = &(list->head);
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    //在其后方插入。
    node->next = p->next;
    node->prev = p;
    p->next = node;
    //如果本身p就是tail的话，将tail后移。
    if(list->tail==p)
    {
        list->tail = list->tail->next;
    }
    return;
}
void insert_existing_inline_node_before_existing(inline_linked_list* list,inline_linked_list_node* node,inline_linked_list_node* existing)
{
    //插入。
    node->prev = existing->prev;
    node->next = existing;
    if(existing != &(list->head)) {
        existing->prev->next = node;
        existing->prev = node;
    }
        
}
void remove_inline_node_pos(inline_linked_list* list,int pos)
{
    int i=0;
    inline_linked_list_node* p = list->head.next;
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    remove_inline_node(list,p);
}
void remove_inline_node(inline_linked_list* list,inline_linked_list_node* node)
{
    if(list->tail == node)
    {
        node->prev->next = nullptr;
        list->tail = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
}




void init_iterator_linked_list(linked_list_iterator* iterator,linked_list* source)
{
    iterator->list = source;
    iterator->current = &(source->head);
    iterator->next = (void*)iterator_linked_list_next;
    iterator->prev = (void*)iterator_linked_list_prev;
    iterator->remove = (void*)iterator_linked_list_remove;
}
