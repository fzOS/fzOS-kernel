#include <common/spinlock.h>
#include <types.h>
void acquire_spinlock(spinlock* lock)
{ 
    while(!__sync_val_compare_and_swap(lock,0,1)) {
        __asm__ ("pause");
    }     
}
void release_spinlock(spinlock* lock)
{
    *lock=0;
}
