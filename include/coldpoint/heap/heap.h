#ifndef HEAP_H
#define HEAP_H
#include <coldpoint/common/class.h>
typedef struct {
    const char* signature;
    const char* typename;
    U64 value;
} ObjectVar;
typedef struct {
    class* parent_class;
    ObjectVar var[0];
} object;
void new_object(class* c);
#endif
