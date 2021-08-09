#ifndef QUEUE_H
#define QUEUE_H
#include <types.h>

typedef struct
{
    U64 bufsize;
    U64 in_position;
    U64 out_position;
    U8 buf[0];
}__attribute__((packed)) queue;

U64 queue_in(queue* q,U8* in,U64 count);
U64 queue_out(queue* q,U8* buf,U64 count);
U64 queue_size(queue* q);
U8 queue_out_single(queue* q);
void queue_in_single(queue* q,U8 d);
#endif
