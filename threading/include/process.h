#ifndef PROCESS_H
#define PROCESS_H
//基本的进程状态。
#define PROCESS_UNAVAILABLE 0
#define PROCESS_RUNNING 1
#define PROCESS_BLOCKED 2
#define PROCESS_READY 3
#include <types.h>
#include <linkedlist.h>

typedef struct {
    U64 pid;
    linked_list threads;
    
} process;
#endif