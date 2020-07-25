#ifndef ITERATOR
#define ITERATOR
#define iterator(type) \
struct iterator_##type \
{ \
    type* list; \
    type##_node* current; \
    int (*next)(struct iterator_##type* this); \
    int (*prev)(struct iterator_##type* this); \
    void (*remove)(struct iterator_##type* this); \
} 
#define init_iterator(type,iterator,source) init_iterator_##type(iterator,source)
#endif