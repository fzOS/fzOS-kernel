#ifndef BLOCKDEV
#define BLOCKDEV
#include <types.h>
typedef struct {
    void (*readblock)(U64 offset, void* data);
    void (*writeblock)(U64 offset,void* data);
    U64 block_size;
} block_dev;
#endif
