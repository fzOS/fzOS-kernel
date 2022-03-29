#ifndef HEAP_H
#define HEAP_H
#include <coldpoint/common/class.h>
typedef struct {
    const char* signature;
    const char* typename;
    const char* class;
    U64 value;
} ObjectVar;
typedef struct {
    class* parent_class;
    U32 var_count;
    ObjectVar var[0];
} object;
object* new_object(class* c);
U64 get_parameter_in_object(object* o,const char* signature,const char* typename,const char* class);
void set_parameter_in_object(object* o,const char* signature,const char* typename,const char* class,U64 val);
void* allocate_heap(U64 size);
#endif
