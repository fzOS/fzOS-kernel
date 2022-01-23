#include <filesystem/efivarfs.h>
#include <memory/memory.h>
#include <common/file.h>
#include <common/printk.h>
typedef enum {
    OPEN_BY_UUID=0,
    OPEN_BY_NAME=1,
}EfiVarOpenMethod;
int efivarfs_mount(EFI_RUNTIME_SERVICES *rt)
{
    EFIvarfsTreeNode* node = (EFIvarfsTreeNode*)device_tree_resolve_by_path("/EFIVariables",nullptr,DT_CREATE_IF_NONEXIST);
    memset(&node->fs,0,sizeof(node->fs));
    node->node.type = DT_FILESYSTEM;
    node->fs.rt = rt;
    node->fs.generic.open    = efivarfs_open;
    node->fs.generic.read    = efivarfs_read;
    node->fs.generic.write   = efivarfs_write;
    node->fs.generic.seek    = efivarfs_seek;
    node->fs.generic.close   = efivarfs_close;
    node->fs.generic.unmount = efivarfs_unmount;
    return FzOS_SUCCESS;
}
int efivarfs_open(struct FzOSFileSystem* fs,char* filename,struct file* file)
{
    EFIvarfsFileSystem* fsl =(EFIvarfsFileSystem*)fs;
    printk("Opening %s.\n",filename);
    if(!memcmp(filename+1,"ByName",6)) {
        printk("By Name.\n");
        file->fs_internal_parameter[2] = OPEN_BY_NAME;
        if(*(filename+7)=='\0') { //open the whole dir:actually nothing.
            file->fs_internal_parameter[1]=file->fs_internal_parameter[0]=0;
            file->type   = FILE_TYPE_DIRECTORY;
            file->offset = 0;
            file->size   = 0; //We do not know the size...
            return FzOS_SUCCESS;
        }
        if(*(filename+7)=='/') { //open file.
            //fsl->rt->
            return FzOS_SUCCESS;
        }
        goto not_found;
    }
    else if(!memcmp(filename+1,"ByUuid",6)) {
        printk("By UUID.\n");
        file->fs_internal_parameter[2] = OPEN_BY_UUID;
        if(*(filename+7)=='\0') { //open the whole dir:actually nothing.
            file->fs_internal_parameter[1]=file->fs_internal_parameter[0]=0;
            file->type   = FILE_TYPE_DIRECTORY;
            file->offset = 0;
            file->size   = 0; //We do not know the size...
            return FzOS_SUCCESS;
        }
        if(*(filename+7)=='/') { //open file.
            return FzOS_SUCCESS;
        }
        goto not_found;
    }
    else
not_found:
        return FzOS_FILE_NOT_FOUND;
}
int efivarfs_read(struct file* file,void* buf,U64 buflen)
{

}
int efivarfs_seek(struct file* file,U64 offset,SeekDirection direction)
{

}
int efivarfs_write(struct file* file,void* buf,U64 buflen)
{

}
int efivarfs_close(struct file* f)
{
    memset(f,0,sizeof(file));
    return FzOS_SUCCESS;
}
int efivarfs_unmount(FzOSFileSystem* fs)
{
    //no need to unmount.
    return FzOS_SUCCESS;
}
