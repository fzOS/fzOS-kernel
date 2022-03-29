#ifndef NATIVE_FILE
#define NATIVE_FILE
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
cpstatus file_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus file_class_register(InlineLinkedList* loaded_class_list);
#endif
