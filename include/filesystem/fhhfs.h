#ifndef FHHFS_H
#define FHHFS_H
#include <filesystem/filesystem.h>
#include <drivers/gpt.h>
#include <common/file.h>
//从我一年之前的作业那里嫖来的。
//Salute!
#define FHHFS_CRC_MAGIC_NUMBER 189050311

typedef struct {
    filesystem generic;
    U64 node_size;
    U64 physical_blocks_per_node;
    U64 node_total;
    U64 node_used;
    U64 node_table_entry;
}fhhfs_filesystem;
typedef struct {
    device_tree_node node;
    fhhfs_filesystem fs;
} fhhfs_tree_node;
typedef struct {
    U8  magic_id[7]; //"fhhfs!" + '\0',0x00~0x06
    U8  version:7; //文件系统版本，0x07
    U8  dirty_mark:1; //"文件系统脏"标记，当文件系统未被正确卸载时，此标记为1，与version共用一个字节
    U64 last_mount_timestamp; //上一次挂载时间戳，0x08~0x0F
    U64 node_total; //文件系统中总共的节点个数，0x10~0x17
    U64 node_used; //文件系统中已被使用的节点个数，0x18~0x1F
    U64 main_node_table_entry; //文件系统的主节点分配表位置。
    U64 back_node_table_entry; //文件系统的备份节点分配表位置。
    U16 node_size; //文件系统中的块大小，0x20~0x21
    U8  label[30]; //文件系统卷标
    U8  end_sign[2]; //结束标志,(55AA)，0x22~0x23
}__attribute__((packed))  fhhfs_magic_head;
/*
    定义一个基本的文件节点的物理结构格式。
    在fhhfs中，文件节点只负责维护block间的引用关系(链表)。
    当next为0时，该节点没有后继节点。
    当next具有实际值时，在读取数据时将会自动跳转至下一节点，并将数据附在当前节点的数据之后。
*/
typedef struct {
    // unsigned long long next; //隐式链表已被废弃。
    byte data[0];//写0是为了运行时确定。
}  __attribute__((packed)) node;
/*
    定义一个基本文件节点的逻辑结构格式。
    在fhhfs中，逻辑结构独立于物理结构。
    （这样只需第一个物理结构存储文件信息就好了）
    （四舍五入就是省了一个T啊哈哈哈哈）
*/
typedef struct {
    U64 create_timestamp;
    U64 modify_timestamp;
    U64 open_timestamp;
    FileType file_type:4; //文件类型，0:普通文件,1:目录文件,2:块设备,3:软链接,其他：还没想好
    U32 owner_priv:4; //拥有者的权限+Set UID
    U32 group_priv:4; //用户所在的群组的权限+Set GID
    U32 other_priv:4; //其他用户的权限+Sticky
    U32 user_id;
    U32 group_id;
    U64 filesize;
} __attribute__((packed)) fhhfs_file_header;

int fhhfs_mount(GPTPartition* partition,const char* destination);
int fhhfs_open(struct filesystem* fs,char* filename,struct file* file);
int fhhfs_read(struct file* file,void* buf,U64 buflen);
int fhhfs_seek(struct file* file,U64 offset,SeekDirection direction);
int fhhfs_write(struct file* file,void* buf,U64 buflen);
int fhhfs_close(struct file* file);
int fhhfs_unmount(filesystem* fs);
#endif
