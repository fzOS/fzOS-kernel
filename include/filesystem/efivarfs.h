#ifndef EFIVARFS_H
#define EFIVARFS_H
#include <types.h>
#include <filesystem/filesystem.h>
#include <uefivars.h>
typedef struct {
    FzOSFileSystem generic;
    EFI_RUNTIME_SERVICES *rt;
}EFIvarfsFileSystem;
typedef struct {
    DeviceTreeNode node;
    EFIvarfsFileSystem fs;
} EFIvarfsTreeNode;
int efivarfs_mount(EFI_RUNTIME_SERVICES *rt);
int efivarfs_open(struct FzOSFileSystem* fs,char* filename,struct file* file);
int efivarfs_read(struct file* file,void* buf,U64 buflen);
int efivarfs_seek(struct file* file,U64 offset,SeekDirection direction);
int efivarfs_write(struct file* file,void* buf,U64 buflen);
int efivarfs_close(struct file* file);
int efivarfs_unmount(FzOSFileSystem* fs);

#endif
