#ifndef BLOCKDEV
#define BLOCKDEV
#include <types.h>
typedef struct block_dev{
    //没有问题时，返回0.
    //出错时，返回其他整数。
    int (*readblock)( struct block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount);
    int (*writeblock)(struct block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount);
    U64 block_size;
} block_dev;
#endif
