#include <coldpoint/classloader.h>
#include <types.h>
#include <common/bswap.h>
#include <common/file.h>
#include <common/printk.h>
#include <memory/memory.h>
int access_flags[] = {ACCESS_ABSTRACT,ACCESS_ANNOTATION,ACCESS_BRIDGE,ACCESS_ENUM,\
                      ACCESS_FINAL,ACCESS_INTERFACE,ACCESS_NATIVE,ACCESS_PRIVATE,\
                      ACCESS_PROTECTED,ACCESS_PUBLIC,ACCESS_STATIC,ACCESS_STRICT,\
                      ACCESS_SUPER,ACCESS_SYNTHETIC,ACCESS_VARARGS};
char* access_flag_names[] = {"abstract","annotation","bridge","enum",\
                             "final","interface","native","private",\
                             "protected","public","static","strict",\
                             "super","synthetic","varargs"
};
class* loadclass(void* buf)
{
    U8* p = buf;
    //首先，校验class的文件头。
    if(*(U32*)p== __builtin_bswap32(0xCAFEBABE)) {
        p+=sizeof(U32);
        printk("Version %d.%d.\n",__builtin_bswap16(*((U16*)(p+2))),__builtin_bswap16(*((U16*)p)));
        p+=sizeof(U16)*2;
        U16 const_pool_count = __builtin_bswap16(*((U16*)p));
        printk("Constant Pool Count:%d\n",const_pool_count);
        p+=sizeof(U16);
        ClassConstantType const_type=0;
        for(U16 i=1;i<const_pool_count;i++) {
            const_type=*p;
            p++;
            printk("Const #%d,",i);
            printk("%b %b %b\n",*p,*(p+1),*(p+2));
            switch(const_type) {
                case CONSTANT_UTF8:{
                    U16 size = __builtin_bswap16(*((U16*)p));
                    p+=sizeof(U16);
                    printk("type=utf-8,size:%d,val=",size);
                    for(U16 j=0;j<size;j++) {
                        printk("%c",*p);
                        p++;
                    }
                    printk("\n");
                    break;
                }
                case CONSTANT_INT: {
                    printk("type=int,val=%d\n",__builtin_bswap32(*((U32*)p)));
                    p+=sizeof(U32);
                    break;
                }
                case CONSTANT_FLOAT: {
                    printk("type=float,val=%f\n",bswap32f(*((float*)p)));
                    p+=sizeof(float);
                    break;
                }
                case CONSTANT_LONG: {
                    printk("type=long,val=%ld\n",__builtin_bswap64(*((U64*)p)));
                    p+=sizeof(U64);
                    break;
                }
                case CONSTANT_DOUBLE: {
                    printk("type=double,val=%lf\n",bswap64f(*((double*)p)));
                    p+=sizeof(double);
                    break;
                }
                case CONSTANT_CLASS: {
                    printk("type=class,index=%d\n",__builtin_bswap16(*((U16*)p)));
                    p+=sizeof(U16);
                    break;
                }
                case CONSTANT_STRING: {
                    printk("type=string,val=%d\n",__builtin_bswap16(*((U16*)p)));
                    p+=sizeof(U16);
                    break;
                }
                case CONSTANT_FIELDREF: {
                    printk("type=field,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_METHODREF: {
                    printk("type=method,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_INTERFACEMETHODREF: {
                    printk("type=interface method,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_NAMEANDTYPE: {
                    printk("type=name and type,class name index=%d,type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    p+=sizeof(U16)*2;
                    break;
                }
            }
        }
        U16 access_flag = __builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("flags:");
        for(int j=0;j<15;j++) {
            if(access_flag&access_flags[j]) {
                printk("%s ",access_flag_names[j]);
            }
        }
        printk("\n");
        U16 class=__builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("This class:%d\n",class);
        class=__builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("Super class:%d\n",class);
        U16 interface_count=__builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("Interfaces:%d\n",interface_count);
        //FIXME:interface!
        U16 field_count=__builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("Fields:%d\n",field_count);
        if(field_count>0) {
            for(int j=0;j<field_count;j++) {
                U16 access_flag = __builtin_bswap16(*((U16*)p));
                printk("flags:");
                for(int j=0;j<15;j++) {
                    if(access_flag&access_flags[j]) {
                        printk("%s ",access_flag_names[j]);
                    }
                }
                U16 attributes_count = __builtin_bswap16(*((U16*)(p+6)));
                printk(",name index:%d,descriptor index:%d,attributes_count:%d\n",\
                    __builtin_bswap16(*((U16*)(p+2))),\
                    __builtin_bswap16(*((U16*)(p+4))),attributes_count
                );
                p+=sizeof(U16)*4;
                if(attributes_count>0) {
                    for(int k=0;k<attributes_count;k++) {
                        U32 attribute_length=__builtin_bswap32(*((U32*)(p+2)));
                        printk("name:%d,length:%d, value:",__builtin_bswap16(*((U16*)p)),attribute_length);
                        p += sizeof(U16)+sizeof(U32);
                        for(int l=0;l<attribute_length;l++) {
                            printk("%b ",*p);
                            p++;
                        }
                        printk("\n");
                    }
                }

            }
        }
        U16 method_count=__builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("Methods:%d\n",method_count);
            if(method_count>0) {
            for(int j=0;j<method_count;j++) {
                U16 access_flag = __builtin_bswap16(*((U16*)p));
                printk("flags:");
                for(int j=0;j<15;j++) {
                    if(access_flag&access_flags[j]) {
                        printk("%s ",access_flag_names[j]);
                    }
                }
                U16 attributes_count = __builtin_bswap16(*((U16*)(p+6)));
                printk(",name index:%d,descriptor index:%d,attributes_count:%d\n",\
                    __builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))),\
                    __builtin_bswap16(*((U16*)(p+4))),attributes_count
                );
                p+=sizeof(U16)*4;
                if(attributes_count>0) {
                    for(int k=0;k<attributes_count;k++) {
                        U32 attribute_length=__builtin_bswap32(*((U32*)(p+2)));
                        printk("name:%d,length:%d, value:",__builtin_bswap16(*((U16*)p)),attribute_length);
                        p += sizeof(U16)+sizeof(U32);
                        for(int l=0;l<attribute_length;l++) {
                            printk("%b ",*p);
                            p++;
                        }
                        printk("\n");
                    }
                }
            }
        }

    }
    return nullptr;
}
class* loadclass_ng(void* buf)
{
    class* c = memalloc(PAGE_SIZE-sizeof(U64));
    memset(c,0,sizeof(class));
    U64 size_needed = sizeof(class);
    U64 size_allocated = PAGE_SIZE-sizeof(U64);
    U8* p = buf;
//     U64 buffer_size=0;
    //首先，校验class的文件头。
   if(*(U32*)p== __builtin_bswap32(0xCAFEBABE)) {
        p+=sizeof(U32);
        printk("Version %d.%d.\n",__builtin_bswap16(*((U16*)(p+2))),__builtin_bswap16(*((U16*)p)));
        p+=sizeof(U16)*2;
        U16 const_pool_count = __builtin_bswap16(*((U16*)p));
        c->constant_pool_entry_count = const_pool_count;
        c->constant_entry_offset = 0;
        printk("Constant Pool Count:%d\n",const_pool_count);
        size_needed += sizeof(constant_entry)*const_pool_count;
        if(size_needed>=size_allocated) {
            size_allocated += PAGE_SIZE;
            c = memrealloc(c,size_allocated);
        }
        p+=sizeof(U16);
        constant_entry* c_entry = (constant_entry*)((c->buffer)+c->constant_entry_offset);
        U64 buffer_off=sizeof(constant_entry)*const_pool_count;
        ClassConstantType const_type=0;
        for(U16 i=1;i<const_pool_count;i++) {
            c_entry[i].type=*p;
            c_entry[i].offset=buffer_off;
            const_type=*p;
            p++;
            printk("Const #%d,",i);
            switch(const_type) {
                case CONSTANT_UTF8:{
                    U16 size = __builtin_bswap16(*((U16*)p));
                    *((U16*)(c->buffer+buffer_off)) = size;
                    p+=sizeof(U16);
                    buffer_off+=sizeof(U16);
                    printk("type=utf-8,size:%d,val=",size);
                    for(U16 j=0;j<size;j++) {
                        *((c->buffer+buffer_off)) = *p;
                        printk("%c",*p);
                        p++;
                        buffer_off++;
                    }
                    printk("\n");
                    break;
                }
                case CONSTANT_INT: {
                    printk("type=int,val=%d\n",__builtin_bswap32(*((U32*)p)));
                    *((U32*)(c->buffer+buffer_off)) = __builtin_bswap32(*((U32*)p));
                    p+=sizeof(U32);
                    buffer_off+=sizeof(U32);
                    break;
                }
                case CONSTANT_FLOAT: {
                    printk("type=float,val=%f\n",bswap32f(*((float*)p)));
                    *((float*)(c->buffer+buffer_off)) = bswap32f(*((float*)p));
                    p+=sizeof(float);
                    buffer_off+=sizeof(float);
                    break;
                }
                case CONSTANT_LONG: {
                    printk("type=long,val=%ld\n",__builtin_bswap64(*((U64*)p)));
                    *((U64*)(c->buffer+buffer_off)) = __builtin_bswap64(*((U64*)p));
                    p+=sizeof(U64);
                    buffer_off+=sizeof(U64);
                    break;
                }
                case CONSTANT_DOUBLE: {
                    printk("type=double,val=%lf\n",bswap64f(*((double*)p)));
                    *((double*)(c->buffer+buffer_off)) = bswap64f(*((double*)p));
                    p+=sizeof(double);
                    buffer_off+=sizeof(double);
                    break;
                }
                case CONSTANT_CLASS: {
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    printk("type=class,index=%d\n",__builtin_bswap16(*((U16*)p)));
                    p+=sizeof(U16);
                    buffer_off+=sizeof(U16);
                    break;
                }
                case CONSTANT_STRING: {
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    printk("type=string,val=%d\n",__builtin_bswap16(*((U16*)p)));
                    p+=sizeof(U16);
                    buffer_off+=sizeof(U16);
                    break;
                }
                case CONSTANT_FIELDREF: {
                    printk("type=field,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    buffer_off+=sizeof(U16);
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p+2));
                    buffer_off+=sizeof(U16);
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_METHODREF: {
                    printk("type=method,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    buffer_off+=sizeof(U16);
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p+2));
                    buffer_off+=sizeof(U16);
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_INTERFACEMETHODREF: {
                    printk("type=interface method,class index=%d,name&type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    buffer_off+=sizeof(U16);
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p+2));
                    buffer_off+=sizeof(U16);
                    p+=sizeof(U16)*2;
                    break;
                }
                case CONSTANT_NAMEANDTYPE: {
                    printk("type=name and type,class name index=%d,type index=%d\n",__builtin_bswap16(*((U16*)p)),__builtin_bswap16(*((U16*)(p+2))));
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p));
                    buffer_off+=sizeof(U16);
                    *((U16*)(c->buffer+buffer_off)) = __builtin_bswap16(*((U16*)p+2));
                    buffer_off+=sizeof(U16);
                    p+=sizeof(U16)*2;
                    break;
                }
            }
        }
        U16 access_flag = __builtin_bswap16(*((U16*)p));
        p+=sizeof(U16);
        printk("flags:%x\n",access_flag);

    }
    return c;
}
int init_classloader(void)
{
    file file;
    int ret;
    ret = generic_open("/Init.class",&file);
    if(ret !=FzOS_SUCEESS) {
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
    class* c = loadclass_ng(buf);
    printk(" Class consant count:%d.\n",c->constant_pool_entry_count);
    constant_entry* entry = (constant_entry*)(c->buffer+c->constant_entry_offset);
    for(int i=0;i<c->constant_pool_entry_count;i++) {
        printk("Entry #%d,type %d,%b%b%b.\n",i,entry[i].type,entry[i].offset,\
                                            (c->buffer[entry[i].offset]),\
                                            (c->buffer[entry[i].offset+1]),\
                                            (c->buffer[entry[i].offset+2])
        );
    }

    (void)entry;
    return FzOS_SUCEESS;
}
