#ifndef CONSOLE
#define CONSOLE
#include <drivers/chardev.h>
#include <limit.h>
#include <common/queue.h>
#include <common/semaphore.h>
typedef struct {
    queue queue;
    U8 buffer[BUFFER_MAX];
} ConsoleBufferQueue;
//定义终端的数据结构类型。
typedef struct {
    CharDev common;
    //一个输入，一个输出。
    //输出缓冲。
    ConsoleBufferQueue input_buffer;
    ConsoleBufferQueue output_buffer;
    //两个信号量。
    semaphore input_sem;
    semaphore output_sem;
} Console;
//貌似别的……也没了？
#endif
