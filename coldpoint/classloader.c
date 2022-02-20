#include <coldpoint/classloader.h>
#include <types.h>
#include <common/bswap.h>
#include <common/file.h>
#include <common/printk.h>
#include <memory/memory.h>
#include <coldpoint/threading/thread.h>
static InlineLinkedList g_loaded_class_linked_list = {
    .tail = &g_loaded_class_linked_list.head
};
class* loadclass(void* class_file)
{
    ClassLinkedListNode* node = memalloc(PAGE_SIZE-sizeof(U64));
    memset(node,0,PAGE_SIZE-sizeof(U64));
    U64 allocated_size = PAGE_SIZE;
    U64 needed_size = sizeof(class);
    U8* p = class_file;
    if(*(U32*)p== bswap32(0xCAFEBABE)) {
        p+=sizeof(U32);
        p+=sizeof(U16)*2;
        U16 const_pool_count = bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.constant_entry_offset=0;
        node->c.constant_pool_entry_count = const_pool_count;
        node->c.buffer_size += sizeof(ConstantEntry)*const_pool_count;
        needed_size+=sizeof(ConstantEntry)*const_pool_count;
        if(needed_size>=allocated_size) {
            allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
            node = memrealloc(node,allocated_size);
        }
        node->c.constant_entry_offset=0;
        ConstantEntry* const_entry = (ConstantEntry*)(node->c.buffer+node->c.constant_entry_offset);
        ConstantEntry* this_const_entry;
        for(int i=1;i<const_pool_count;i++) {
            this_const_entry = &const_entry[i];
            this_const_entry->type=*p;
            p++;
            switch(this_const_entry->type) {
                case CONSTANT_UTF8:{
                    ((UTF8InfoConstant*)this_const_entry)->length = bswap16(*((U16*)p));
                    ((UTF8InfoConstant*)this_const_entry)->buffer_offset = node->c.buffer_size;
                    p+=sizeof(U16);
                    needed_size += ((UTF8InfoConstant*)this_const_entry)->length +1;
                    if(needed_size>=allocated_size) {
                        allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                        node = memrealloc(node,allocated_size);
                        const_entry = (ConstantEntry*)(node->c.buffer+node->c.constant_entry_offset);
                        this_const_entry = &const_entry[i];
                    }
                    for(U16 j=0;j<((UTF8InfoConstant*)this_const_entry)->length;j++) {
                        node->c.buffer[node->c.buffer_size]=*p;
                        p++;
                        node->c.buffer_size++;
                    }
                    //结尾补上C风格\0，确保可以被直接printk.
                    node->c.buffer[node->c.buffer_size]='\0';
                    node->c.buffer_size++;
                    break;
                }
                case CONSTANT_INT: {
                    ((IntegerInfoConstant*)this_const_entry)->val = bswap32(*((U32*)p));
                    p+=sizeof(U32);
                    break;
                }
                case CONSTANT_FLOAT: {
                    ((FloatInfoConstant*)this_const_entry)->val = bswap32f(*((float*)p));
                    p+=sizeof(float);
                    break;
                }
                case CONSTANT_LONG: {
                    ((LongInfoConstant*)this_const_entry)->val = bswap64(*((U64*)p));
                    p+=sizeof(U64);
                    //long和double占用2个表项。
                    //有一说一这玩意没有十年脑血栓不可能设计出这么个鬼东西……
                    i++;
                    break;
                }
                case CONSTANT_DOUBLE: {
                    ((DoubleInfoConstant*)this_const_entry)->val = bswap64f(*((double*)p));
                    p+=sizeof(double);
                    //long和double占用2个表项。
                    //有一说一这玩意没有十年脑血栓不可能设计出这么个鬼东西……
                    i++;
                    break;
                }
                case CONSTANT_CLASS: {
                    ((ClassInfoConstant*)this_const_entry)->name_index = bswap16(*((U16*)p));
                    p+=sizeof(U16);
                    break;
                }
                case CONSTANT_STRING: {
                    ((StringInfoConstant*)this_const_entry)->string_index = bswap16(*((U16*)p));
                    p+=sizeof(U16);
                    break;
                }
                case CONSTANT_FIELDREF: {
                    ((FieldRefConstant*)this_const_entry)->class_index = bswap16(*((U16*)p));
                    ((FieldRefConstant*)this_const_entry)->name_and_type_index = bswap16(*((U16*)(p+2)));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_METHODREF: {
                    ((MethodRefConstant*)this_const_entry)->class_index = bswap16(*((U16*)p));
                    ((MethodRefConstant*)this_const_entry)->name_and_type_index = bswap16(*((U16*)(p+2)));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_INTERFACEMETHODREF: {
                    ((InterfaceRefConstant*)this_const_entry)->class_index = bswap16(*((U16*)p));
                    ((InterfaceRefConstant*)this_const_entry)->name_and_type_index = bswap16(*((U16*)(p+2)));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_NAMEANDTYPE: {
                    ((NameAndTypeInfoConstant*)this_const_entry)->name_index = bswap16(*((U16*)p));
                    ((NameAndTypeInfoConstant*)this_const_entry)->descriptor_index = bswap16(*((U16*)(p+2)));
                    p+=sizeof(U16)*2;
                    break;
                }
                default: {
                    p++;
                }
            }
        }
        node->c.access_flag = bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.this_class=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.super_class=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.interface_pool_entry_count=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.interface_pool_entry_offset = node->c.buffer_size;
        if(node->c.interface_pool_entry_count) {
            node->c.buffer_size+=sizeof(U16)*const_pool_count;
            needed_size+=sizeof(U16)*const_pool_count;
            if(needed_size>=allocated_size) {
                allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                node = memrealloc(node,allocated_size);
            }
            U16* interface_index = (U16*)&(node->c.buffer[node->c.interface_pool_entry_offset]);
            for(int j=0;j<node->c.interface_pool_entry_count;j++) {
                *interface_index = bswap16(*((U16*)p));
                p+=sizeof(U16);
                interface_index++;
            }
        }
        node->c.fields_pool_entry_count=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.fields_pool_entry_offset = node->c.buffer_size;
        if(node->c.fields_pool_entry_count) {
            node->c.buffer_size+=sizeof(FieldInfoEntry)*node->c.fields_pool_entry_count;
            needed_size+=sizeof(FieldInfoEntry)*node->c.fields_pool_entry_count;
            if(needed_size>=allocated_size) {
                allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                node = memrealloc(node,allocated_size);
            }
            FieldInfoEntry* field_entry = (FieldInfoEntry*)&(node->c.buffer[node->c.fields_pool_entry_offset]);
            for(int j=0;j<node->c.fields_pool_entry_count;j++) {
                field_entry->access_flags = bswap16(*((U16*)p));
                field_entry->name_index = bswap16(*((U16*)(p+2)));
                field_entry->descriptor_index =  bswap16(*((U16*)(p+4)));
                field_entry->attribute_count = bswap16(*((U16*)(p+6)));
                field_entry->attribute_info_entry_offset = node->c.buffer_size;
                p+=sizeof(U16)*4;
                if(field_entry->attribute_count) {
                    node->c.buffer_size+=sizeof(AttributeInfoEntry)*field_entry->attribute_count;
                    needed_size+=sizeof(AttributeInfoEntry)*field_entry->attribute_count;
                    if(needed_size>=allocated_size) {
                        allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                        node = memrealloc(node,allocated_size);
                    }
                    AttributeInfoEntry* attribute_entry = (AttributeInfoEntry*)&(node->c.buffer[field_entry->attribute_info_entry_offset]);
                    for(int k=0;k<field_entry->attribute_count;k++) {
                        attribute_entry->attribute_name_index = bswap16(*((U16*)p));
                        attribute_entry->attribute_length = bswap32(*((U32*)(p+2)));
                        attribute_entry->info_offset = node->c.buffer_size;
                        p += sizeof(U16)+sizeof(U32);
                        node->c.buffer_size+=attribute_entry->attribute_length;
                        needed_size+=attribute_entry->attribute_length;
                        if(needed_size>=allocated_size) {
                            allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                            node = memrealloc(node,allocated_size);
                        }
                        for(int l=0;l<attribute_entry->attribute_length;l++) {
                            node->c.buffer[node->c.buffer_size]=*p;
                            p++;
                            node->c.buffer_size++;
                        }
                        attribute_entry++;
                    }
                }
                field_entry++;
            }

        }

        node->c.method_pool_entry_count=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.method_pool_entry_offset = node->c.buffer_size;
        if(node->c.method_pool_entry_count) {
            node->c.buffer_size+=sizeof(MethodInfoEntry)*node->c.method_pool_entry_count;
            needed_size+=sizeof(MethodInfoEntry)*node->c.method_pool_entry_count;
            if(needed_size>=allocated_size) {
                allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                node = memrealloc(node,allocated_size);
            }
            MethodInfoEntry* method_entry = (MethodInfoEntry*)&(node->c.buffer[node->c.method_pool_entry_offset]);
            for(int j=0;j<node->c.method_pool_entry_count;j++) {
                method_entry->access_flags = bswap16(*((U16*)p));
                method_entry->name_index = bswap16(*((U16*)(p+2)));
                method_entry->descriptor_index =  bswap16(*((U16*)(p+4)));
                method_entry->attribute_count = bswap16(*((U16*)(p+6)));
                method_entry->attribute_info_entry_offset = node->c.buffer_size;
                p+=sizeof(U16)*4;
                if(method_entry->attribute_count) {
                    node->c.buffer_size+=sizeof(AttributeInfoEntry)*method_entry->attribute_count;
                    needed_size+=sizeof(AttributeInfoEntry)*method_entry->attribute_count;
                    if(needed_size>=allocated_size) {
                        allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                        node = memrealloc(node,allocated_size);
                    }
                    AttributeInfoEntry* attribute_entry = (AttributeInfoEntry*)&(node->c.buffer[method_entry->attribute_info_entry_offset]);
                    for(int k=0;k<method_entry->attribute_count;k++) {
                        attribute_entry->attribute_name_index = bswap16(*((U16*)p));
                        attribute_entry->attribute_length = bswap32(*((U32*)(p+2)));
                        attribute_entry->info_offset = node->c.buffer_size;
                        p += sizeof(U16)+sizeof(U32);
                        needed_size+=attribute_entry->attribute_length;
                        if(needed_size>=allocated_size) {
                            allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                            node = memrealloc(node,allocated_size);
                        }
                        for(int l=0;l<attribute_entry->attribute_length;l++) {
                            node->c.buffer[node->c.buffer_size]=*p;
                            p++;
                            node->c.buffer_size++;
                        }
                        attribute_entry++;
                    }
                }
                method_entry++;
            }
        }
        node->c.class_attributes_entry_count=bswap16(*((U16*)p));
        p+=sizeof(U16);
        node->c.class_attributes_entry_offset = node->c.buffer_size;
        if(node->c.class_attributes_entry_count) {
            node->c.buffer_size+=sizeof(AttributeInfoEntry)*node->c.class_attributes_entry_count;
            needed_size+=sizeof(AttributeInfoEntry)*node->c.class_attributes_entry_count;
            if(needed_size>=allocated_size) {
                allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                node = memrealloc(node,allocated_size);
            }
            AttributeInfoEntry* class_attribute_entry = (AttributeInfoEntry*)&(node->c.buffer[node->c.class_attributes_entry_offset]);
            for(int k=0;k<node->c.class_attributes_entry_count;k++) {
                class_attribute_entry->attribute_name_index = bswap16(*((U16*)p));
                class_attribute_entry->attribute_length = bswap32(*((U32*)(p+2)));
                class_attribute_entry->info_offset = node->c.buffer_size;
                p += sizeof(U16)+sizeof(U32);
                needed_size+=class_attribute_entry->attribute_length;
                if(needed_size>=allocated_size) {
                    allocated_size = ((needed_size/PAGE_SIZE)+1)*PAGE_SIZE;
                    node = memrealloc(node,allocated_size);
                }
                for(int l=0;l<class_attribute_entry->attribute_length;l++) {
                    node->c.buffer[node->c.buffer_size]=*p;
                    p++;
                    node->c.buffer_size++;
                }
                class_attribute_entry++;
            }
        }
    }
    node->c.class_name = class_get_utf8_string(&node->c,class_get_class_name_index(&node->c,node->c.this_class));
    insert_existing_inline_node(&g_loaded_class_linked_list,&node->node,-1);
    return &node->c;
}

int init_classloader(void)
{

    file file;
    int ret;
    ret = generic_open("/A.class",&file);
    if(ret !=FzOS_SUCCESS) {
        printk(" Open Init fail: %d!\n",ret);
        return FzOS_ERROR;
    }
    void* buf = memalloc(file.size);
    ret = file.filesystem->read(&file,buf,file.size);
    if(ret==0) {
        memfree(buf);
        printk(" Read Init fail: %d!\n",ret);
        return FzOS_ERROR;
    }
    class* c = loadclass(buf);
    memfree(buf);
     print_class_info(c);
     print_class_constants(c);
     print_field_and_method_info(c);
     thread_test(c);
    return FzOS_SUCCESS;
}
