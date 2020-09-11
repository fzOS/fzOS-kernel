#ifndef MUTEX_H
#define MUTEX_H
#include <types.h>
typedef volatile U32 mutex;
void acquire_mutex(mutex* mutex);
void release_mutex(mutex* mutex);
#endif