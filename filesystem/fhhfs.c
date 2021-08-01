#include <filesystem/fhhfs.h>
#include <drivers/gpt.h>
#include <common/printk.h>
#include <memory/memory.h>
int fhhfs_mount(GPTPartition* partition,const char* destination)
{
    void* buf = allocate_page(1);
    //FIXME:Rev of mkfs.
    partition->header.readblock(&partition->header,3,buf,PAGE_SIZE,1);
    fhhfs_magic_head* head = (fhhfs_magic_head*) buf;
    if(!memcmp(head->magic_id,"fhhfs!",7)) {
        printk(" Got correct fhhfs:%s,%d nodes /%d bytes in total.\n",head->label,head->node_total,head->node_total*head->node_size);
        return FzOS_SUCEESS;
    }
    return FzOS_ERROR;
}

int fhhfs_open(char* filename,struct file* file)
{
    return FzOS_SUCEESS;
}
int fhhfs_read(struct file* file,void* buf,U64 buflen)
{
    U64 count = 0;

    return count;
}
int fhhfs_seek(struct file* file,U64 offset,SeekDirection direction)
{
    return FzOS_SUCEESS;
}
int fhhfs_close(struct file* file)
{
    return FzOS_SUCEESS;
}
