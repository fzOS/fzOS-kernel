#ifndef FS_GENERIC
#define FS_GENERIC
#include <types.h>
#include <drivers/gpt.h>
struct file;
typedef enum {
    SEEK_FROM_BEGINNING,
    SEEK_FROM_END,
    SEEK_FROM_CURRENT
}SeekDirection;

extern const GUID* registered_filesystems[];
extern int (*registered_filesystem_mounts[])(GPTPartition* partition,const char* position);
extern char root_device_path[64];


typedef struct FzOSFileSystem{
    int (*open)(struct FzOSFileSystem* fs,char* filename,struct file* file);
    int (*read)(struct file* file,void* buf,U64 buflen);
    int (*seek)(struct file* file,U64 offset,SeekDirection direction);
    int (*write)(struct file* file,void* buf,U64 buflen);
    int (*close)(struct file* file);
    int (*unmount)(struct FzOSFileSystem* fs);
    block_dev* dev;
} FzOSFileSystem;

typedef struct {
    DeviceTreeNode node;
    FzOSFileSystem fs;
} FileSystemTreeNode;
int mount(GPTPartition* partition,const char* position);
int unmount_all_mounted_filesystems();
int mount_root_partition();
#endif
