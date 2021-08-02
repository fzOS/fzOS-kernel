#include <filesystem/fhhfs.h>
#include <drivers/gpt.h>
#include <common/printk.h>
#include <memory/memory.h>
#include <common/file.h>
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
    fhhfs_filesystem* fsl = (fhhfs_filesystem*)fs;
    printk(" Opening %s;begin at %x.\n",filename,fsl->node_table_entry);
    file->filesystem = fs;

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
