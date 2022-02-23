#ifndef CLASSLOADER_H
#define CLASSLOADER_H
#include <coldpoint/common/class.h>
#include <common/linkedlist.h>
typedef struct {
    InlineLinkedListNode node;
    class c;
} ClassLinkedListNode;
int init_classloader(void);
class* loadclass(void* class_file);
class* getclass(const U8* class_name);
#endif
