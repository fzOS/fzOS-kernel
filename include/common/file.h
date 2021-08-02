#ifndef FILE_H
#define FILE_H
#include <filesystem/filesystem.h>
#include <drivers/blockdev.h>
typedef struct file{
    block_dev* device;
    U64 size;
    U64 offset;
    U64 entry_node;
    filesystem filesystem; //必须放在最后！不然多态不能做。
} file;
#endif
