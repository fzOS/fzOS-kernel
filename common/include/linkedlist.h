#ifndef LINKEDLIST
#define LINKEDLIST
#include <types.h>
typedef struct linked_list_node 
{
    struct linked_list_node* prev;
    void* data;
    struct linked_list_node* next;
} linked_list_node;

typedef struct  
{
    //带头结点，head->next为第一个数据。
    linked_list_node head;
    //尾结点就是最后一个数据。
    linked_list_node* tail;
} linked_list;
void insert_existing_node(linked_list* list,linked_list_node* node,int pos);
void remove_node(linked_list* list,linked_list_node* node);
void remove_node_pos(linked_list* list,int pos);
#endif