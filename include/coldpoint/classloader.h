#ifndef CLASSLOADER_H
#define CLASSLOADER_H
#include <coldpoint/common/class.h>
int init_classloader(void);
class* loadclass(void* buf);
#endif
