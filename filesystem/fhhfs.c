#include <filesystem/fhhfs.h>
#include <drivers/gpt.h>
#include <common/printk.h>
#include <memory/memory.h>
#include <common/file.h>
#include <common/kstring.h>

#include <drivers/sata_ahci.h>
#include <common/random.h>
#include <common/crc32.h>

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
inline int fhhfs_readnode(FhhfsFilesystem* fs,U64 node_id,void* buffer,U64 block_count)
{
    return fs->generic.dev->readblock(fs->generic.dev,
                       node_id*fs->physical_blocks_per_node,
                       buffer,block_count*fs->node_size,block_count*fs->physical_blocks_per_node);
}
inline int fhhfs_writenode(FhhfsFilesystem* fs,U64 node_id,void* buffer,U64 block_count)
{
    return fs->generic.dev->writeblock(fs->generic.dev,
                       node_id*fs->physical_blocks_per_node,
                       buffer,block_count*fs->node_size,block_count*fs->physical_blocks_per_node);
}
inline int fhhfs_stat(FhhfsFilesystem* fs,file* file)
{
    //file中的node必须存在。
    FhhfsFileHeader* buf = allocate_page(1);
    fhhfs_readnode(fs,file->fs_internal_parameter[0],buf,1);
    file->type = buf->file_type;
    file->offset = 0;
    file->size = buf->filesize;
    free_page(buf,1);
    return FzOS_SUCCESS;
}

//这里的buffer共享时可以节省内存分配开销。
U64 fhhfs_get_next_node_id(FhhfsFilesystem* fs,U64 prev_id,void* buffer) {
    U64 dest_node_page = prev_id/(fs->node_size/sizeof(U64));
    fhhfs_readnode(fs,dest_node_page+fs->node_table_entry,buffer,1);
    return ((U64*)buffer)[prev_id%(fs->node_size/sizeof(U64))];
}

//定义写入下个id的函数。
int fhhfs_write_node_id(FhhfsFilesystem* fs,U64 node,U64 value,void* buf)
{
    //首先，查找node分配表.
    U64 dest_node_page = node/(fs->node_size/sizeof(U64));
    fhhfs_readnode(fs,(fs->node_table_entry+dest_node_page),buf,1);
    //写入数据。
    U64* node_id_page = buf;
    node_id_page[node%(2048/sizeof(unsigned long long))] = value;
    fhhfs_writenode(fs,(fs->node_table_entry+dest_node_page),buf,1);
    return FzOS_SUCCESS;
}
//定义分配新节点的函数。
//当占用率小于30%时，首个节点采取随机分配。
//当占用率大于30%时，首个节点采取顺序分配。
int fhhfs_allocate_node(FhhfsFilesystem* fs,U64* dest,int n)
{
    //如果剩余空间不足，那么直接退出。
    if(fs->node_used+n>fs->node_total) {
        return FzOS_NO_SPACE_LEFT;
    }
    unsigned long long tmp;
    void* buf = allocate_page(1);
    if(fs->node_used*1.0/fs->node_total>0.3) {
        //顺序分配。
        tmp = 0;
        while(fhhfs_get_next_node_id(fs,++tmp,buf)!=0);
    }
    else {
        //随机分配。
        while(fhhfs_get_next_node_id(fs,(tmp = (random_get_u16()*1.0/0x10000)*fs->node_total,tmp),buf)!=0);
    }
    for(int i=0;i<n;i++) {
        dest[i]=tmp;
        while(fhhfs_get_next_node_id(fs,++tmp,buf)!=0) {
            if(tmp >= fs->node_total) {
                tmp = tmp - fs->node_total;
            }
        }
    }
    if(n>1) {
        for(int i=0;i<n-1;i++) {
            fhhfs_write_node_id(fs,dest[i],(dest[i+1]),buf);
        }
        fhhfs_write_node_id(fs,dest[n-1],1,buf);
    }
    else {
        fhhfs_write_node_id(fs,dest[0],1,buf);
    }
    free_page(buf,1);
    return FzOS_SUCCESS;
}

int fhhfs_mount(GPTPartition* partition,const char* destination)
{
    void* buf = allocate_page(1);
    partition->header.readblock(&partition->header,0,buf,PAGE_SIZE,1);
    FhhfsMagicHead* head = (FhhfsMagicHead*) buf;
    if(!memcmp(head->magic_id,"fhhfs!",7)) {
        if(head->dirty_mark) {
            printk(" %s:File system is dirty.Remember to unmount next time.\n",destination);
        }
        //挂载时写入“文件系统脏”标记。
        head->dirty_mark=1;
        I32* crc = (I32*)(buf+sizeof(FhhfsMagicHead));
        *crc = crc32(FHHFS_CRC_MAGIC_NUMBER,buf,sizeof(FhhfsMagicHead));
        partition->header.writeblock(&partition->header,0,buf,PAGE_SIZE,1);
        FhhfsTreeNode* new_node = allocate_page((sizeof(FhhfsTreeNode)/PAGE_SIZE)+1);
        new_node->fs.generic.open = fhhfs_open;
        new_node->fs.generic.read = fhhfs_read;
        new_node->fs.generic.write = fhhfs_write;
        new_node->fs.generic.close = fhhfs_close;
        new_node->fs.generic.seek = fhhfs_seek;
        new_node->fs.generic.unmount = fhhfs_unmount;
        new_node->fs.generic.dev = &(partition->header);
        new_node->fs.node_table_entry = head->main_node_table_entry;
        new_node->fs.node_size = head->node_size;
        new_node->fs.physical_blocks_per_node = head->node_size / partition->parent->block_size;
        new_node->fs.node_total = head->node_total;
        new_node->fs.node_used = head->node_used;
        new_node->node.type = DT_FILESYSTEM;
        //如果有之前的设备树结点，换下来。
        DeviceTreeNode* old_node = device_tree_resolve_by_path(destination,nullptr,DT_CREATE_IF_NONEXIST);
        memcpy(new_node->node.name,old_node->name,DT_NAME_LENGTH_MAX);
        device_tree_replace_node(old_node,&(new_node->node),DT_DESTROY_AFTER_REPLACE);

        return FzOS_SUCCESS;
    }
    free_page(buf,1);
    return FzOS_ERROR;
}
int fhhfs_unmount(FzOSFileSystem* fs)
{
    FhhfsFilesystem* fsl = (FhhfsFilesystem*) fs;
    void* buf = allocate_page(1);
    fhhfs_readnode(fsl,0,buf,1);
    FhhfsMagicHead* head = (FhhfsMagicHead*) buf;
    head->dirty_mark = 0;
    I32* crc = (I32*)(buf+sizeof(FhhfsMagicHead));
    *crc = crc32(FHHFS_CRC_MAGIC_NUMBER,buf,sizeof(FhhfsMagicHead));
    fhhfs_writenode(fsl,0,buf,1);
    free_page(buf,1);
    return FzOS_SUCCESS;
}
int fhhfs_open(FzOSFileSystem* fs,char* filename,struct file* file)
{
    //输入格式：
    //以/开头的地址。
    //例如： “/”，“/test”,"/dir/a"
    FhhfsFilesystem* fsl = (FhhfsFilesystem*)fs;
    file->filesystem = fs;
    //开始循环解析。
    U64 buf_size = 1;
    file->fs_internal_parameter[0] = 1;//“/”
    char bufname[FILENAME_MAX];
    file->fs_internal_parameter[0] = 1;
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
        fhhfs_read(file,buf,file->size);
        file->fs_internal_parameter[0] = fhhfs_get_node_from_dir(bufname,buf,file->size);
        if(file->fs_internal_parameter[0] == FzOS_FILE_NOT_FOUND) {
                free_page(buf,buf_size);
                return FzOS_FILE_NOT_FOUND;
        }
        fhhfs_stat(fsl,file);
    }
    while(buf_str_len=strmid(bufname,FILENAME_MAX,p,PATH_SEPARATOR),buf_str_len);
open_finish:
    free_page(buf,buf_size);
    return FzOS_SUCCESS;
}
int fhhfs_read(struct file* file,void* buf,U64 buflen)
{
    U64 count = 0;
    FhhfsFilesystem* fsl = (FhhfsFilesystem*)file->filesystem;
    if(file->offset >= file->size) {
        return FzOS_ERROR;
    }
    U64 file_offset = file->offset+sizeof(FhhfsFileHeader);
    count = (buflen>(file->size-file->offset))?(file->size-file->offset):buflen;
    U64 node_count = (((file_offset%fsl->node_size)+count)/fsl->node_size)+1;
    void* read_buffer = allocate_page(node_count / (PAGE_SIZE/fsl->node_size)+1);
    void* fat_buffer = allocate_page(1);
    U64 begin_node = (file_offset)/fsl->node_size;
    U64 current_node = file->fs_internal_parameter[0];
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
int fhhfs_seek(struct file* file,U64 offset,SeekDirection direction)
{
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
int fhhfs_close(struct file* f)
{
    memset(f,0,sizeof(file));
    return FzOS_SUCCESS;
}
int fhhfs_write(struct file* file,void* buf,U64 buflen)
{
    U64 count = 0;
    FhhfsFilesystem* fsl = (FhhfsFilesystem*)file->filesystem;
    //由于Block device的特点，需要读出原始块，覆盖，写入原始块。
    U64 file_offset = file->offset+sizeof(FhhfsFileHeader);
    //count = (buflen>(file->size-file->offset))?(file->size-file->offset):buflen;
    I64 node_count = (((file_offset%fsl->node_size)+count)/fsl->node_size)+1;
    void* read_buffer = allocate_page(1);//由于只要写入，因此只要写一块就行了。
    void* fat_buffer = allocate_page(1);
    U64 begin_node = (file_offset)/fsl->node_size;
    U64 current_node = file->fs_internal_parameter[0];
    while(begin_node--) {
        current_node = fhhfs_get_next_node_id(fsl,current_node,fat_buffer);
    }
    void* bufp = buf;
    //然后，把涉及到的block依次读进来。
    while(node_count--) {
        U64 override_size = (fsl->node_size-(file_offset%fsl->node_size))<=buflen?
                            (fsl->node_size-(file_offset%fsl->node_size)):buflen;
        fhhfs_readnode(fsl,current_node,read_buffer,1);
        memcpy(read_buffer+(file_offset%fsl->node_size),bufp,override_size);
        fhhfs_writenode(fsl,current_node,read_buffer,1);
        file_offset+=override_size;
        current_node = fhhfs_get_next_node_id(fsl,current_node,fat_buffer);
        if(current_node==1) {
            break;
        }
    }
    if(node_count>0) {
        U64 new_node_pos[node_count];
        fhhfs_allocate_node(fsl,new_node_pos,node_count);
        for(int i=0;i<node_count;i++) {
            U64 override_size = (fsl->node_size-(file_offset%fsl->node_size))<=buflen?
                                (fsl->node_size-(file_offset%fsl->node_size)):buflen;
            memcpy(read_buffer+(file_offset%fsl->node_size),bufp,override_size);
            fhhfs_writenode(fsl,new_node_pos[i],read_buffer,1);
            file_offset+=override_size;
        }
    }
    file->offset+=buflen;
    return FzOS_SUCCESS;
}
