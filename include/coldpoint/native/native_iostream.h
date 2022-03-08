#ifndef NATIVE_IOSTREAM
#define NATIVE_IOSTREAM
#include <coldpoint/automata/automata.h>
#include <coldpoint/native/nativehandler.h>
const char * const g_iostream_class_name  = "fzos/base/IOStream";
cpstatus iostream_class_entry(thread* t,const U8* method_name,const U8* method_type,NativeClassOperations operations);
#endif
