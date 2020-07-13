#include <linkedlist.h>
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