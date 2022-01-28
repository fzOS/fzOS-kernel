#ifndef LINKEDLIST
#define LINKEDLIST
#include <types.h>
#include <common/iterator.h>
typedef struct LinkedListNode
{
    struct LinkedListNode* prev;
    void* data;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct InlineLinkedListNode
{
    struct InlineLinkedListNode* prev;
    struct InlineLinkedListNode* next;
} InlineLinkedListNode;
typedef struct  
{
    //带头结点，head->next为第一个数据。
    LinkedListNode head;
    //尾结点就是最后一个数据。
    LinkedListNode* tail;
} LinkedList;
typedef struct  
{
    //带头结点，head->next为第一个数据。
    InlineLinkedListNode head;
    //尾结点就是最后一个数据。
    InlineLinkedListNode* tail;
} InlineLinkedList;


typedef IteratorRaw(LinkedList) IteratorLinkedList;
typedef IteratorRaw(InlineLinkedList) IteratorInlineLinkedList;

void insert_existing_node(LinkedList* list,LinkedListNode* node,int pos);
void insert_existing_node_before_existing(LinkedList* list,LinkedListNode* node,LinkedListNode* existing);
void remove_node(LinkedList* list,LinkedListNode* node);
void remove_node_pos(LinkedList* list,int pos);
void insert_existing_inline_node(InlineLinkedList* list,InlineLinkedListNode* node,int pos);
void insert_existing_inline_node_before_existing(InlineLinkedList* list,InlineLinkedListNode* node,InlineLinkedListNode* existing);
void remove_inline_node(InlineLinkedList* list,InlineLinkedListNode* node);
void remove_inline_node_pos(InlineLinkedList* list,int pos);
void init_iterator_LinkedList(Iterator(LinkedList)* iterator,LinkedList* source);
void init_iterator_InlineLinkedList(Iterator(InlineLinkedList)* iterator,InlineLinkedList* source);
#endif
