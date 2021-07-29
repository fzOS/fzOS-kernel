#ifndef GPT_H
#define GPT_H
#include <drivers/blockdev.h>
typedef struct {
    char signature[8];//EFI PART
    U8  revision[4];//0x00 0x01 0x00 0x00
    U32 header_size;
    U32 crc32;
    U8  reserved[4];
    U64 lba_this_header;
    U64 lba_alternative_header;
    U64 lba_first_usable;
    U64 lba_last_usable;
    U8  disk_guid[16];
    U64 partition_entry_lba;
    U32 partition_entry_count;
    U32 partition_entry_size;
    U32 entry_array_crc32;
}__attribute__((packed)) GPTHeader;
typedef struct {
    U8 partition_type_guid[16];
    U8 partition_guid[16];
    U64 starting_lba;
    U64 ending_lba;
    U64 attributes;
    U8 partname[72];
}__attribute__((packed)) GPTEntry;

extern const GUID FzOS_ROOT_PARTITION_GUID;

int get_gpt_partition_count(block_dev* dev);
#endif
