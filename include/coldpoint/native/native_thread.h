#ifndef NATIVE_THREAD
#define NATIVE_THREAD

#include <coldpoint/native/nativehandler.h>
cpstatus thread_class_entry(thread* t, const U8* name, const U8* desc, NativeClassOperations operations);
cpstatus thread_class_register(InlineLinkedList* loaded_class_list);
#endif
