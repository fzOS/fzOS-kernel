#include <common/semaphore.h>
void acquire_semaphore(semaphore* sem)
{
    while(*sem<=0) {
        //TODO:giveup();
        __asm__ ("hlt");
    }
    __sync_fetch_and_sub(sem,1);
}
void release_semaphore(semaphore* sem)
{
    //no need to check/wait.
    __sync_fetch_and_add(sem,1);
}
