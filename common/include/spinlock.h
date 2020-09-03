#ifndef SPINLOCK_H
#define SPINLOCK_H
typedef volatile int spinlock;
void acquire_spinlock(spinlock* lock);
void release_spinlock(spinlock* lock);
#endif