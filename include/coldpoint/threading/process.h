#ifndef PROCESS_H
#define PROCESS_H
#include <types.h>
#include <common/queue.h>
#include <common/semaphore.h>
typedef struct {
    U64 pid;
    struct {
        queue q;
        U8 buf[128];
        semaphore input_sem;
        U64* pending_buffer;
    } input_buffer;
} process;

#endif
