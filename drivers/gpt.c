#include <drivers/gpt.h>
#include <memory/memory.h>
#include <common/kstring.h>
#include <drivers/devicetree.h>

#include <drivers/sata_ahci.h>
const GUID FzOS_ROOT_PARTITION_GUID = {
    .first = 0x12345678,
    .second = 0x8765,
    .third = 0x4321,
    .fourth = {0x23,0x33},
    .fifth = {0x66,0x66,0x66,0x66,0x66,0x66}
};
static const char* g_partition_name_template = "Partition%d";
int gpt_readblock(BlockDev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount)
{
    GPTPartition* partition = (GPTPartition*)dev;
    if(offset+blockcount>(partition->end_lba)) {
        return FzOS_POSITION_OVERFLOW;
    }
    return partition->parent->readblock(partition->parent,offset+partition->begin_lba,buffer,buffer_size,blockcount);
}
int gpt_writeblock(BlockDev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount)
{
    GPTPartition* partition = (GPTPartition*)dev;
    if(offset+blockcount>(partition->end_lba)) {
        return FzOS_POSITION_OVERFLOW;
    }
    return partition->parent->writeblock(partition->parent,offset+partition->begin_lba,buffer,buffer_size,blockcount);
}

int gpt_partition_init(BlockDev* dev,DeviceTreeNode* parent)
{
    //首先，分配缓冲，读一块。
    char* buffer = allocate_page(1);
    dev->readblock(dev,1,buffer,dev->block_size,1);
    GPTHeader* header = (GPTHeader*)buffer;
    if(memcmp(header->signature,"EFI PART",8)) {
        //不是GPT分区表！
        return FzOS_ERROR;
    }
    int i,j;
    int entry_count_in_a_block = dev->block_size/header->partition_entry_size;
    U64 gpt_entry_count = header->partition_entry_count;
    U64 gpt_entry_begin_lba = header->partition_entry_lba;
    U64 ret = FzOS_SUCCESS;
    //直接在这里就初始化了。
    GPTPartitionTreeNode* node;
    char buf[DT_NAME_LENGTH_MAX];
    GPTEntry* entry_buffer = (GPTEntry*)header;
    GPTEntry* entry;
    for(i=0;i<gpt_entry_count/entry_count_in_a_block;i++) {
        dev->readblock(dev,gpt_entry_begin_lba+i,buffer,dev->block_size,1);
        for(j=0;j<entry_count_in_a_block;j++) {
            entry = entry_buffer+j;
            if(!memcmp(entry->partition_type_guid,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",16)) {
                goto out;
            }
            node = allocate_page((sizeof(GPTPartitionTreeNode)-1)/PAGE_SIZE +1);
            memset(node,0,sizeof(GPTPartitionTreeNode));
            node->node.type = DT_BLOCK_DEVICE;
            sprintk(buf,g_partition_name_template,i*entry_count_in_a_block+j);
            memcpy(node->node.name,buf,DT_NAME_LENGTH_MAX);
            node->partition.parent = dev;
            node->partition.begin_lba = entry->starting_lba;
            node->partition.end_lba   = entry->ending_lba;
            memcpy(&node->partition.type,entry->partition_type_guid,sizeof(GUID));
            node->partition.header.block_size=dev->block_size;
            node->partition.header.readblock = gpt_readblock;
            node->partition.header.writeblock = gpt_writeblock;
            device_tree_add_from_parent(&node->node,parent);
            if(!memcmp(entry->partition_type_guid,&FzOS_ROOT_PARTITION_GUID,16)) {
                ret = (i*entry_count_in_a_block+j) | FzOS_ROOT_PARTITION_FOUND;
            }

        }
    }
out:
    free_page(buffer,1);
    return ret;
}

