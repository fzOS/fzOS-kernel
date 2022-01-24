#include <filesystem/efivarfs.h>
#include <memory/memory.h>
#include <common/file.h>
#include <common/kstring.h>
const GUID FzOS_VENDOR_GUID = {
    .first = 0x1234ABCD,
    .second = 0xBEEF,
    .third = 0xBABE,
    .fourth = {0x23,0x33},
    .fifth = {0x88,0x88,0x88,0x88,0x88,0x88}
};
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
    /*
      format:
      GUID-Name
      XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX-SomeVar
    */
    EFIvarfsFileSystem* fsl =(EFIvarfsFileSystem*)fs;
    if(filename[1]=='\0') { //Open ROOT dir.
        file->filesystem = fs;
        file->type = FILE_TYPE_DIRECTORY;
        file->offset = 0;
        file->size = 0;
        return FzOS_SUCCESS;
    }
    //Print all.
    CHAR16 var_name[FILENAME_MAX];
    char short_name[FILENAME_MAX];
    char combined_name[FILENAME_MAX];
    UINTN  var_name_size = FILENAME_MAX;
    memset(var_name,0,FILENAME_MAX);
    EFI_GUID guid;
    EFI_STATUS ret=EFI_SUCCESS;
    while(1) {
        ret = fsl->rt->GetNextVariableName(&var_name_size,var_name,&guid);
        if(ret!=EFI_SUCCESS) {
            goto not_found;
        }
        shrink_lstring_to_string(var_name,var_name_size,short_name);
        sprintk(combined_name,"%g-%s",guid,short_name);
        if(!strcomp(combined_name,filename+1)) {
            file->filesystem = fs;
            file->type = FILE_TYPE_FILE;
            file->offset = 0;
            memcpy(file->fs_internal_parameter,&guid,sizeof(GUID));
            void* name_buffer = memalloc(FILENAME_MAX);
            memcpy(name_buffer,var_name,FILENAME_MAX);
            file->fs_internal_parameter[2] = (U64)name_buffer;
            file->fs_internal_parameter[3] = 0;
            //Get Size.
            UINTN size=0;
            ret = fsl->rt->GetVariable(var_name,&guid,nullptr,&size,combined_name);
            if(ret==EFI_BUFFER_TOO_SMALL) {
                file->size = size;
                return FzOS_SUCCESS;
            }
            goto not_found;
        }
        var_name_size = FILENAME_MAX;
    }
not_found:
        return FzOS_FILE_NOT_FOUND;
}
int efivarfs_read(struct file* file,void* buf,U64 buflen)
{
    void* buffer = memalloc(file->size);
    UINTN file_size = file->size;
    U64 read_count = (buflen<file->size-file->offset?buflen:file->size-file->offset);
    EFIvarfsFileSystem* fsl = (EFIvarfsFileSystem*)file->filesystem;
    EFI_GUID guid;
    UINT32 params;
    memcpy(&guid,file->fs_internal_parameter,sizeof(EFI_GUID));
    EFI_STATUS ret = fsl->rt->GetVariable((CHAR16*)file->fs_internal_parameter[2],
                                          &guid,
                                          &params,
                                          &file_size,
                                          buffer);

    if(ret!=EFI_SUCCESS) {
        memfree(buffer);
        return FzOS_ERROR;
    }
    memcpy(buf,buffer+file->offset,read_count);
    file->fs_internal_parameter[3] = params;
    memfree(buffer);
    file->offset += read_count;
    return read_count;
}
int efivarfs_seek(struct file* file,U64 offset,SeekDirection direction)
{
    //Is it really necessary to seek()……?
    switch(direction) {
        case SEEK_FROM_BEGINNING:{
            file->offset = offset;
            if(file->size<=offset) {
                file->offset = file->size;
                return FzOS_POSITION_OVERFLOW;
            }
            break;
        }
        case SEEK_FROM_CURRENT: {
            file->offset += offset;
            if(file->size<=offset) {
                file->offset = file->size;
                return FzOS_POSITION_OVERFLOW;
            }
            break;
        }
        case SEEK_FROM_END: {
            file->offset = file->size-offset;
            if(file->offset<0) {
                file->offset = 0;
                return FzOS_POSITION_OVERFLOW;
            }
            break;
        }
    }
    return FzOS_SUCCESS;
}
int efivarfs_write(struct file* file,void* buf,U64 buflen)
{
    EFIvarfsFileSystem* fsl = (EFIvarfsFileSystem*)file->filesystem;
    //First read, then write.
    UINTN actual_buffer_size_needed = (file->offset+buflen>=file->size?file->offset+buflen:file->size);
    U64 orig_offset = file->offset;
    void* buffer = memalloc(actual_buffer_size_needed);
    file->offset = 0;
    if(efivarfs_read(file,buffer,file->size)!=FzOS_ERROR) {
        memcpy(buffer+orig_offset,buf,buflen);
        EFI_GUID guid;
        UINT32 params;
        memcpy(&guid,file->fs_internal_parameter,sizeof(EFI_GUID));
        params = (UINT32) file->fs_internal_parameter[3];
        EFI_STATUS ret = fsl->rt->SetVariable((CHAR16*)file->fs_internal_parameter[2],
                                          &guid,
                                          params,
                                          actual_buffer_size_needed,
                                          buffer);

        if(ret!=EFI_SUCCESS) {
            if(ret==EFI_WRITE_PROTECTED) {
                ret = FzOS_READ_ONLY;
            }
            else {
                ret=FzOS_ERROR;
            }
            memfree(buffer);
            return ret;
        }
        file->size = actual_buffer_size_needed;
    }
    memfree(buffer);
    file->offset = orig_offset;
    return 0;
}
int efivarfs_close(struct file* f)
{
    memfree((void*)f->fs_internal_parameter[2]);
    memset(f,0,sizeof(file));
    return FzOS_SUCCESS;
}
int efivarfs_unmount(FzOSFileSystem* fs)
{
    //no need to unmount.
    return FzOS_SUCCESS;
}
