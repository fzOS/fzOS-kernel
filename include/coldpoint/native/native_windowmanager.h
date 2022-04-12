#ifndef NATIVE_WINDOWMANAGER
#define NATIVE_WINDOWMANAGER
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
#include <coldpoint/heap/heap.h>
#include <zcrystal/window_manager.h>
typedef struct {
    object o;
    Window* w;
} WindowObject;
cpstatus window_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus windowmanager_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus windowmanager_class_register(InlineLinkedList* loaded_class_list);
#endif
