#ifndef FS_GENERIC
#define FS_GENERIC
#include <types.h>
#include <drivers/gpt.h>
struct file;
typedef enum {
    SEEK_FROM_BEGINNING,
    SEEK_FROM_END
}SeekDirection;

extern const GUID* registered_filesystems[];
extern int (*registered_filesystem_mounts[])(GPTPartition* partition,const char* position);
extern char root_device_path[64];


typedef struct filesystem{
    int (*open)(struct filesystem* fs,char* filename,struct file* file);
    int (*read)(struct filesystem* fs,struct file* file,void* buf,U64 buflen);
    int (*seek)(struct filesystem* fs,struct file* file,U64 offset,SeekDirection direction);
    int (*close)(struct filesystem* fs,struct file* file);
    block_dev* dev;
} filesystem;

typedef struct {
    device_tree_node node;
    filesystem fs;
} file_system_tree_node;
int mount(GPTPartition* partition,const char* position);
int mount_root_partition();
#endif
