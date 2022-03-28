#ifndef NATIVE_IOSTREAM
#define NATIVE_IOSTREAM
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
cpstatus iostream_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus iostream_class_register(InlineLinkedList* loaded_class_list);
#endif
