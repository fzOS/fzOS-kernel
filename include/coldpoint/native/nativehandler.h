#ifndef NATIVE_HANDLER_H
#define NATIVE_HANDLER_H
#include <coldpoint/automata/automata.h>
typedef enum {
    NATIVE_GETSTATIC,
    NATIVE_PUTSTATIC,
    NATIVE_GETFIELD,
    NATIVE_PUTFIELD,
    NATIVE_INVOKE
}NativeClassOperations;
typedef cpstatus (NativeClassEntry)(thread* t,const U8* name,const U8* type,NativeClassOperations operations);
extern const char* const g_native_class_names[];
extern NativeClassEntry* const g_native_class_entries[];
#endif
