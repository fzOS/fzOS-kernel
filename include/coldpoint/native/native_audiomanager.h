#ifndef NATIVE_AUDIOSTREAM
#define NATIVE_AUDIOSTREAM
#include <coldpoint/automata/automata.h>
#include <coldpoint/heap/heap.h>
#include <common/wav.h>
#include <coldpoint/native/nativehandler.h>
cpstatus audiomanager_class_entry(thread* t, const U8* name, const U8* type, NativeClassOperations operations);
cpstatus audiomanager_class_register(InlineLinkedList* loaded_class_list);

#endif
