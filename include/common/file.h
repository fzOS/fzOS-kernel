#ifndef FILE_H
#define FILE_H
#include <filesystem/filesystem.h>
#include <drivers/blockdev.h>
typedef struct file{
    filesystem* filesystem;
    U64 size;
    U64 offset;
    U64 entry_node;
} file;
#endif
