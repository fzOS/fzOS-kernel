#ifndef CONSOLE
#define CONSOLE
#include <drivers/chardev.h>
#include <limit.h>
#include <common/queue.h>
#include <common/semaphore.h>
typedef struct {
    queue queue;
    U8 buffer[BUFFER_MAX];
} console_buffer_queue;
//定义终端的数据结构类型。
typedef struct {
    char_dev common;
    //一个输入，一个输出。
    //输出缓冲。
    console_buffer_queue input_buffer;
    console_buffer_queue output_buffer;
    //两个信号量。
    semaphore input_sem;
    semaphore output_sem;
} console;
//貌似别的……也没了？
#endif
