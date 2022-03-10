#ifndef NATIVE_HANDLER_H
#define NATIVE_HANDLER_H
#include <coldpoint/automata/automata.h>
#include <common/linkedlist.h>
#include <stddef.h>
typedef enum {
    NATIVE_GETSTATIC,
    NATIVE_PUTSTATIC,
    NATIVE_GETFIELD,
    NATIVE_PUTFIELD,
    NATIVE_INVOKE,
    NATIVE_NEW
}NativeClassOperations;
typedef cpstatus (NativeClassEntry)(thread* t,const U8* name,const U8* type,NativeClassOperations operations);
typedef cpstatus (NativeClassRegister)(InlineLinkedList* loaded_class_list);
typedef struct {
    const U8* class_name;
    ClassType type;
    NativeClassEntry* entry;
} NativeClass;
typedef struct {
    InlineLinkedListNode node;
    NativeClass c;
} NativeClassInlineLinkedListNode;
_Static_assert(offsetof(NativeClass,class_name)==offsetof(class,class_name)
             &&offsetof(NativeClass,type)==offsetof(class,type),
               "Native Class offset != Class offset!");
extern NativeClassRegister* const g_native_class_register_handlers[];
#endif
