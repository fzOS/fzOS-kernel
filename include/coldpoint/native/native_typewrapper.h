#ifndef NATIVE_TYPE_WRAPPER
#define NATIVE_TYPE_WRAPPER
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
#include <coldpoint/heap/heap.h>
cpstatus typewrapper_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus typewrapper_class_register(InlineLinkedList* loaded_class_list);
typedef struct {
    object o;
    U64 val;
} NativeTypeWrapperObject;
NativeTypeWrapperObject* constant_to_string(const U8* in);
const U8* get_constant_from_string(const NativeTypeWrapperObject* o);
#endif
