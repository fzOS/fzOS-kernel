#include <common/queue.h>
#include <memory/memory.h>
U64 queue_size(queue* q)
{
    return (q->in_position+q->bufsize)%q->bufsize;
}
U64 queue_in(queue* q,U8* in,U64 count)
{
    U64 exceed_size = (q->in_position+count)-q->bufsize;
    if(exceed_size<0) {
        exceed_size = 0;
    }
    memcpy(q->buf+q->in_position,in,count-exceed_size);
    memcpy(q->buf,in+(count-exceed_size),exceed_size);
    q->in_position = (q->in_position+count)%q->bufsize;
    return count;
}
U64 queue_out(queue* q,U8* buf,U64 count)
{
    U64 exceed_size = (q->out_position+count)-q->bufsize;
    if(exceed_size<0) {
        exceed_size = 0;
    }
    memcpy(buf,q->buf+q->out_position,count-exceed_size);
    memcpy(buf+(count-exceed_size),q->buf,exceed_size);
    q->out_position = (q->out_position+count)%q->bufsize;
    return count;
}
void queue_in_single(queue* q,U8 d)
{
    q->buf[q->in_position++] = d;
    if(q->in_position==q->bufsize) {
        q->in_position=0;
    }
}
U8 queue_out_single(queue* q)
{
    U8 ret = q->buf[q->out_position++];
    if(q->out_position==q->bufsize) {
        q->out_position=0;
    }
    return ret;
}
