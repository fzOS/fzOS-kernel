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
    loadclass(buf);
    return FzOS_SUCEESS;
}
