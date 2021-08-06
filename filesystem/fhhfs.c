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
        new_node->fs.physical_blocks_per_node = head->node_size / partition->parent->block_size;
        new_node->fs.node_total = head->node_total;
        new_node->fs.node_used = head->node_used;
        new_node->node.type = DT_FILESYSTEM;
        //如果有之前的设备树结点，换下来。
        device_tree_node* old_node = device_tree_resolve_by_path(destination,DT_CREATE_IF_NONEXIST);
        memcpy(new_node->node.name,old_node->name,DT_NAME_LENGTH_MAX);
        device_tree_replace_node(old_node,&(new_node->node),DT_DESTROY_AFTER_REPLACE);

        //测试。
        file file;
        new_node->fs.generic.open(&(new_node->fs.generic),"/",&file);
        return FzOS_SUCEESS;
    }
    return FzOS_ERROR;
}

int fhhfs_open(filesystem* fs,char* filename,struct file* file)
{
    //输入格式：
    //以/开头的地址。
    //例如： “/”，“/test”,"/dir/a"
    fhhfs_filesystem* fsl = (fhhfs_filesystem*)fs;
    printk(" Opening %s;begin at %x.\n",filename,fsl->node_table_entry);
    file->filesystem = fs;
    //开始循环解析。
    void* buf = allocate_page(1);
    U64 buf_size = 1;
    file->fs_entry_node = 1;//“/”
    fs->dev->readblock(fs->dev,
                       file->fs_entry_node*((fhhfs_filesystem*)fs)->physical_blocks_per_node,
                       buf,PAGE_SIZE,1);
    file_header* header = (file_header*)buf;
    printk("/:size:%d,type:%d,owner:%d\n",header->filesize,header->file_type,header->user_id);
    char bufname[FILENAME_MAX];
    file->fs_entry_node = header->filesize;
    file->type = header->file_type;
    file->offset = 0;
    file->size = header->filesize;
    char* p = bufname;
    int buf_str_len=1; //跳过开头的/
    ls(buf+sizeof(file_header),header->filesize);
    if(!strcomp(filename,"/")) {
        goto open_finish;
    }
    memcpy(bufname,"/",2);
    do {
        p += buf_str_len;
        if(*p==PATH_SEPARATOR) {
            p++;
        }
        if(file->size>buf_size*PAGE_SIZE) {
            //缓冲不够。
            free_page(buf,buf_size);
            buf_size = (file->size/PAGE_SIZE)+1;
            allocate_page(buf_size);
        }
        fhhfs_read(fs,file,buf,buf_size*PAGE_SIZE);
    }
    while(buf_str_len=strmid(buf,FILENAME_MAX,p,PATH_SEPARATOR),buf_str_len);
open_finish:
    free_page(buf,buf_size);
    return FzOS_SUCEESS;
}
int fhhfs_read(filesystem* fs,struct file* file,void* buf,U64 buflen)
{
    U64 count = 0;

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
