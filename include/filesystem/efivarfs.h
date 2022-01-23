#ifndef EFIVARFS_H
#define EFIVARFS_H
#include <types.h>
#include <filesystem/filesystem.h>
#include <uefivars.h>
typedef struct {
    FileSystem generic;
    EFI_RUNTIME_SERVICES *rt;
}EFIvarfsFileSystem;
typedef struct {
    DeviceTreeNode node;
    EFIvarfsFileSystem fs;
} EFIvarfsTreeNode;
int mount_efivars(EFI_RUNTIME_SERVICES *rt);
#endif
