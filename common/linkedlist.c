#include <common/linkedlist.h>
#include <memory/memory.h>
int iterator_linked_list_next(Iterator(LinkedList)* this)
{
    if(this->current==this->list->tail) {
        return 0;
    }
    this->current = this->current->next;
    return 1;
}
int iterator_linked_list_prev(Iterator(LinkedList)* this)
{
    if(this->current==this->list->head.next) {
        return 0;
    }
    this->current = this->current->prev;
    return 1;
}
void iterator_linked_list_remove(Iterator(LinkedList)* this)
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
        this->current = this->current->prev;
    }
    else {
        return;
    }
}
void insert_existing_node(LinkedList* list,LinkedListNode* node,int pos)
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
    LinkedListNode* p = &(list->head);
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
void insert_existing_node_before_existing(LinkedList* list,LinkedListNode* node,LinkedListNode* existing)
{
    //插入。
    node->prev = existing->prev;
    node->next = existing;
    if(existing != &(list->head)) {
        existing->prev->next = node;
        existing->prev = node;
    }
        
}
void remove_node_pos(LinkedList* list,int pos)
{
    int i=0;
    LinkedListNode* p = list->head.next;
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    remove_node(list,p);
}
void remove_node(LinkedList* list,LinkedListNode* node)
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

void insert_existing_inline_node(InlineLinkedList* list,InlineLinkedListNode* node,int pos)
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
    InlineLinkedListNode* p = &(list->head);
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
void insert_existing_inline_node_before_existing(InlineLinkedList* list,InlineLinkedListNode* node,InlineLinkedListNode* existing)
{
    //插入。
    node->prev = existing->prev;
    node->next = existing;
    if(existing != &(list->head)) {
        existing->prev->next = node;
        existing->prev = node;
    }
        
}
void remove_inline_node_pos(InlineLinkedList* list,int pos)
{
    int i=0;
    InlineLinkedListNode* p = list->head.next;
    while(i<pos&&p!=list->tail)
    {
        i++;
        p = p->next;
    }
    remove_inline_node(list,p);
}
void remove_inline_node(InlineLinkedList* list,InlineLinkedListNode* node)
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

int iterator_InlineLinkedList_next(Iterator(InlineLinkedList)* this)
{
    if(this->current==this->list->tail) {
        return 0;
    }
    this->current = this->current->next;
    return 1;
}
int iterator_InlineLinkedList_prev(Iterator(InlineLinkedList)* this)
{
    if(this->current==this->list->head.next) {
        return 0;
    }
    this->current = this->current->prev;
    return 1;
}
void iterator_InlineLinkedList_remove(Iterator(InlineLinkedList)* this)
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
        this->current = this->current->prev;
    }
    else {
        return;
    }
}
void init_iterator_InlineLinkedList(Iterator(InlineLinkedList)* iterator, InlineLinkedList* source)
{
    iterator->list = source;
    iterator->current = &(source->head);
    iterator->next = iterator_InlineLinkedList_next;
    iterator->prev = iterator_InlineLinkedList_prev;
    iterator->remove = iterator_InlineLinkedList_remove;
}

void init_iterator_linked_list(Iterator(LinkedList)* iterator, LinkedList* source)
{
    iterator->list = source;
    iterator->current = &(source->head);
    iterator->next = iterator_linked_list_next;
    iterator->prev = iterator_linked_list_prev;
    iterator->remove = iterator_linked_list_remove;
}
