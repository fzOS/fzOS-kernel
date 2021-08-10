#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <types.h>
typedef volatile I32 semaphore;
void acquire_semaphore(semaphore* sem);
void release_semaphore(semaphore* sem);
#endif
