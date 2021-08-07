#include <filesystem/fhhfs.h>
#include <drivers/gpt.h>
#include <common/printk.h>
#include <memory/memory.h>
#include <common/file.h>
#include <common/kstring.h>

#include <drivers/sata_ahci.h>


//列举出文件下的所有文件。
//测试用。
//注意，buffer中不包含文件头。
void ls(void* buffer,U64 length) {
    void* p = buffer;
    char name[FILENAME_MAX];
    U64 node_id;
    printk("Node ID/Name\n");
    while(p<buffer+length) {
        node_id = *(U64*)(p);
        p+= sizeof(U64);
        p+= strcopy(name,p,FILENAME_MAX);
        printk("%d/%s\n",node_id,name);
    }
}

U64 fhhfs_get_node_from_dir(char* filename,void* buffer,U64 length) {

    void* p = buffer;
    char name[FILENAME_MAX];
    U64 node_id;

    while(p<buffer+length) {
        node_id = *(U64*)(p);
        p+= sizeof(U64);
        p+= strcopy(name,p,FILENAME_MAX);
        if(!strcomp(name,filename)) {
            //找到了！
            return node_id;
        }
    }
    return FzOS_FILE_NOT_FOUND;
}
inline int fhhfs_readnode(fhhfs_filesystem* fs,U64 node_id,void* buffer,U64 block_count)
{
    return fs->generic.dev->readblock(fs->generic.dev,
                       node_id*fs->physical_blocks_per_node,
                       buffer,block_count*fs->node_size,block_count*fs->physical_blocks_per_node);
}
inline int fhhfs_stat(fhhfs_filesystem* fs,file* file)
{
    //file中的node必须存在。
    fhhfs_file_header* buf = allocate_page(1);
    fhhfs_readnode(fs,file->fs_entry_node,buf,1);
    file->type = buf->file_type;
    file->offset = 0;
    file->size = buf->filesize;
    free_page(buf,1);
    return FzOS_SUCEESS;
}
//这里的buffer共享时可以节省内存分配开销。
U64 fhhfs_get_next_node_id(fhhfs_filesystem* fs,U64 prev_id,void* buffer) {
    U64 dest_node_page = prev_id/fs->node_size;
    fhhfs_readnode(fs,dest_node_page+fs->node_table_entry,buffer,1);
    return ((U64*)buffer)[prev_id%(fs->node_size/sizeof(U64))];
}
int fhhfs_mount(GPTPartition* partition,const char* destination)
{
    void* buf = allocate_page(1);
    partition->header.readblock(&partition->header,0,buf,PAGE_SIZE,1);
    fhhfs_magic_head* head = (fhhfs_magic_head*) buf;
    if(!memcmp(head->magic_id,"fhhfs!",7)) {
        printk(" Got correct fhhfs:%s,%d nodes / %d bytes in total.\n",head->label,head->node_total,head->node_total*head->node_size);
        fhhfs_tree_node* new_node = allocate_page((sizeof(fhhfs_tree_node)/PAGE_SIZE)+1);
        new_node->fs.generic.open = fhhfs_open;
        new_node->fs.generic.read = fhhfs_read;
        new_node->fs.generic.close = fhhfs_close;
        new_node->fs.generic.seek = fhhfs_seek;
        new_node->fs.generic.dev = &(partition->header);
        new_node->fs.node_table_entry = head->main_node_table_entry;
        new_node->fs.node_size = head->node_size;
        new_node->fs.physical_blocks_per_node = head->node_size / partition->parent->block_size;
        new_node->fs.node_total = head->node_total;
        new_node->fs.node_used = head->node_used;
        new_node->node.type = DT_FILESYSTEM;
        //如果有之前的设备树结点，换下来。
        device_tree_node* old_node = device_tree_resolve_by_path(destination,DT_CREATE_IF_NONEXIST);
        memcpy(new_node->node.name,old_node->name,DT_NAME_LENGTH_MAX);
        device_tree_replace_node(old_node,&(new_node->node),DT_DESTROY_AFTER_REPLACE);
        free_page(buf,1);
        return FzOS_SUCEESS;
    }
    free_page(buf,1);
    return FzOS_ERROR;
}

int fhhfs_open(filesystem* fs,char* filename,struct file* file)
{
    //输入格式：
    //以/开头的地址。
    //例如： “/”，“/test”,"/dir/a"
    fhhfs_filesystem* fsl = (fhhfs_filesystem*)fs;
    file->filesystem = fs;
    //开始循环解析。
    U64 buf_size = 1;
    file->fs_entry_node = 1;//“/”
    char bufname[FILENAME_MAX];
    file->fs_entry_node = 1;
    fhhfs_stat(fsl,file);
    char* p = filename+1;
    void* buf = allocate_page(1);
    int buf_str_len=1; //跳过开头的/
    if(!strcomp(filename,"/")) {
        goto open_finish;
    }
    buf_str_len=strmid(bufname,FILENAME_MAX,p,PATH_SEPARATOR);
    do {
        p += buf_str_len;
        if(*p==PATH_SEPARATOR) {
            p++;
        }
        if(file->size>buf_size*PAGE_SIZE) {
            //缓冲不够。
            free_page(buf,buf_size);
            buf_size = (file->size/PAGE_SIZE)+1;
            buf=allocate_page(buf_size);
        }
        fhhfs_read(fs,file,buf,file->size);
        file->fs_entry_node = fhhfs_get_node_from_dir(bufname,buf,file->size);
        if(file->fs_entry_node == FzOS_FILE_NOT_FOUND) {
                free_page(buf,buf_size);
                return FzOS_FILE_NOT_FOUND;
        }
        fhhfs_stat(fsl,file);
    }
    while(buf_str_len=strmid(bufname,FILENAME_MAX,p,PATH_SEPARATOR),buf_str_len);
open_finish:
    free_page(buf,buf_size);
    return FzOS_SUCEESS;
}
int fhhfs_read(filesystem* fs,struct file* file,void* buf,U64 buflen)
{
    U64 count = 0;
    fhhfs_filesystem* fsl = (fhhfs_filesystem*)fs;
    if(file->offset >= file->size) {
        return FzOS_ERROR;
    }
    U64 file_offset = file->offset+sizeof(fhhfs_file_header);
    count = (buflen>(file->size-file->offset))?(file->size-file->offset):buflen;
    U64 node_count = (((file_offset%fsl->node_size)+count)/fsl->node_size)+1;
    void* read_buffer = allocate_page(node_count / (PAGE_SIZE/fsl->node_size)+1);
    void* fat_buffer = allocate_page(1);
    U64 begin_node = (file_offset)/fsl->node_size;
    U64 current_node = file->fs_entry_node;
    while(begin_node--) {
        current_node = fhhfs_get_next_node_id(fsl,current_node,fat_buffer);
    }
    void* p = read_buffer;
    //然后，把涉及到的block依次读进来。
    while(node_count--) {
        fhhfs_readnode(fsl,current_node,p,1);
        current_node = fhhfs_get_next_node_id(fsl,current_node,fat_buffer);
        p+=fsl->node_size;
    }
    //数据拷贝回去。
    memcpy(buf,read_buffer+(file_offset%fsl->node_size),count);
    free_page(fat_buffer,1);
    free_page(read_buffer,(node_count / (PAGE_SIZE/fsl->node_size)));
    file->offset+=count;
    return count;
}
int fhhfs_seek(filesystem* fs,struct file* file,U64 offset,SeekDirection direction)
{
    return FzOS_SUCEESS;
}
int fhhfs_close(filesystem* fs,struct file* file)
{
    return FzOS_SUCEESS;
}
