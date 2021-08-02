//注册已知文件系统的GUID.
#include <filesystem/filesystem.h>
#include <filesystem/fhhfs.h>
#include <memory/memory.h>
#include <drivers/devicetree.h>
#include <common/printk.h>
//根目录设备文件的设备树地址。
char root_device_path[64];

const GUID* registered_filesystems[] = {
    &FzOS_ROOT_PARTITION_GUID
};
int (*registered_filesystem_mounts[])(GPTPartition* partition,const char* position) = {
    fhhfs_mount
};
_Static_assert(sizeof(registered_filesystem_mounts)==sizeof(registered_filesystems),"Registered FS types & mounts don't match!");
int mount(GPTPartition* partition,const char* position)
{
    for(int i=0;i<(sizeof(registered_filesystems)/sizeof(GUID*));i++) {
        if(!memcmp(&partition->type,registered_filesystems[i],sizeof(GUID))) {
            return registered_filesystem_mounts[i](partition,position);
        }
    }
    return FzOS_ERROR;
}
int mount_root_partition()
{
    if(root_device_path[0]=='\0') {
        return FzOS_ERROR;
    }
    GPTPartitionTreeNode* partition = (GPTPartitionTreeNode*)device_tree_resolve_by_path(root_device_path,DT_RETURN_IF_NONEXIST);
    if(partition==nullptr) {
        return FzOS_ERROR;
    }

    U64 ret = mount(&partition->partition,"/");
    if(ret == FzOS_SUCEESS) {
        printk(" Got root at %s.\n",root_device_path);
        return FzOS_SUCEESS;
    }
    return FzOS_ERROR;
}
