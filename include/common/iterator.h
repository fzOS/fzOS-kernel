#ifndef ITERATOR
#define ITERATOR
#define iterator_raw(type) \
struct raw_iterator_##type \
{ \
    type* list; \
    type##_node* current; \
    U64 count;\
    int (*next)(struct raw_iterator_##type* this); \
    int (*prev)(struct raw_iterator_##type* this); \
    void (*remove)(struct raw_iterator_##type* this); \
} 
#define iterator(type) iterator_##type
#define init_iterator(type,iterator,source) init_iterator_##type(iterator,source)
#endif
