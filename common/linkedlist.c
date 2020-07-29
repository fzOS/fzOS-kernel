#include <linkedlist.h>
int iterator_linked_list_next(iterator(linked_list)* this)
{
    if(this->current==this->list->tail) {
        return 0;
    }
    this->current = this->current->next;
    return 1;
}
int iterator_linked_list_prev(iterator(linked_list)* this)
{
    if(this->current==this->list->head.next) {
        return 0;
    }
    this->current = this->current->prev;
    return 1;
}
void iterator_linked_list_remove(iterator(linked_list)* this)
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
void init_iterator_linked_list(iterator(linked_list)* iterator,linked_list* source)
{
    iterator->list = source;
    iterator->current = &(source->head);
    iterator->next = (void*)iterator_linked_list_next;
    iterator->prev = (void*)iterator_linked_list_prev;
    iterator->remove = (void*)iterator_linked_list_remove;
}
