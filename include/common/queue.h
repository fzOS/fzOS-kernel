#ifndef QUEUE_H
#define QUEUE_H
#include <types.h>
struct queue;

typedef struct
{
    U64 bufsize;
    U64 in_position;
    U64 out_position;
    U8 buf[0];
}__attribute__((packed)) queue;

void queue_in(queue* q,char* in,U64 count);
void queue_out(queue* q,char* in,U64 count);
#endif
