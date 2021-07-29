#include <drivers/gpt.h>
#include <memory/memory.h>
#include <common/printk.h>
#include <drivers/devicetree.h>
const GUID FzOS_ROOT_PARTITION_GUID = {
    .first = 0x12345678,
    .second = 0x8765,
    .third = 0x4321,
    .fourth = {0x23,0x33},
    .fifth = {0x66,0x66,0x66,0x66,0x66,0x66}
};
int get_gpt_partition_count(block_dev* dev)
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
    GPTEntry* entry_buffer = (GPTEntry*)header;
    GPTEntry* entry;
    for(i=0;i<gpt_entry_count/entry_count_in_a_block;i++) {
        dev->readblock(dev,gpt_entry_begin_lba+i,buffer,dev->block_size,1);
        for(j=0;j<entry_count_in_a_block;j++) {
            entry = entry_buffer+j;
            if(!memcmp(entry->partition_type_guid,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",16)) {
                goto out;
            }
        }
    }
out:
    free_page(buffer,1);
    return 0;
}
