#ifndef FILE_H
#define FILE_H
#include <filesystem/filesystem.h>
#include <drivers/blockdev.h>
typedef enum {
    FILE_TYPE_FILE,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_DEVICE_NODE,
    FILE_TYPE_SYMLINK
} FileType;
typedef struct file{
    filesystem* filesystem;
    U64 size;
    U64 offset;
    U64 fs_entry_node;
    FileType type;
} file;
#endif
