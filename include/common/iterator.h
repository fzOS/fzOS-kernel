#ifndef ITERATOR
#define ITERATOR
#define IteratorRaw(type) \
struct RawIterator##type \
{ \
    type* list; \
    type##Node* current; \
    U64 count;\
    int (*next)(struct RawIterator##type* this); \
    int (*prev)(struct RawIterator##type* this); \
    void (*remove)(struct RawIterator##type* this); \
} 
#define Iterator(type) Iterator##type
#define init_iterator(type,iterator,source) init_iterator_##type(iterator,source)
#endif
