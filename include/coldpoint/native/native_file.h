#ifndef NATIVE_FILE
#define NATIVE_FILE
#include <common/file.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
typedef struct {
    object o;
    file f;
    object* descriptor;
} FileObject;
cpstatus file_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus file_class_register(InlineLinkedList* loaded_class_list);
#endif
