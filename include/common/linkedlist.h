#ifndef LINKEDLIST
#define LINKEDLIST
#include <types.h>
#include <common/iterator.h>

typedef struct linked_list_node 
{
    struct linked_list_node* prev;
    void* data;
    struct linked_list_node* next;
} linked_list_node;

typedef struct inline_linked_list_node 
{
    struct inline_linked_list_node* prev;
    struct inline_linked_list_node* next;
} inline_linked_list_node;
typedef struct  
{
    //带头结点，head->next为第一个数据。
    linked_list_node head;
    //尾结点就是最后一个数据。
    linked_list_node* tail;
} linked_list;
typedef struct  
{
    //带头结点，head->next为第一个数据。
    inline_linked_list_node head;
    //尾结点就是最后一个数据。
    inline_linked_list_node* tail;
} inline_linked_list;
typedef iterator(linked_list) linked_list_iterator;
typedef iterator(inline_linked_list) inline_linked_list_iterator;
void insert_existing_node(linked_list* list,linked_list_node* node,int pos);
void insert_existing_node_before_existing(linked_list* list,linked_list_node* node,linked_list_node* existing);
void remove_node(linked_list* list,linked_list_node* node);
void remove_node_pos(linked_list* list,int pos);
void insert_existing_inline_node(inline_linked_list* list,inline_linked_list_node* node,int pos);
void insert_existing_inline_node_before_existing(inline_linked_list* list,inline_linked_list_node* node,inline_linked_list_node* existing);
void remove_inline_node(inline_linked_list* list,inline_linked_list_node* node);
void remove_inline_node_pos(inline_linked_list* list,int pos);
void init_iterator_linked_list(linked_list_iterator* iterator,linked_list* source);
#endif
