#include <coldpoint/common/class.h>
#include <common/printk.h>
#include <common/bswap.h>
#include <common/kstring.h>
#include <coldpoint/classloader.h>
int g_access_flags[] = {ACCESS_ABSTRACT,ACCESS_ANNOTATION,ACCESS_BRIDGE,ACCESS_ENUM,\
                      ACCESS_FINAL,ACCESS_INTERFACE,ACCESS_NATIVE,ACCESS_PRIVATE,\
                      ACCESS_PROTECTED,ACCESS_PUBLIC,ACCESS_STATIC,ACCESS_STRICT,\
                      ACCESS_SUPER,ACCESS_SYNTHETIC,ACCESS_VARARGS};
char* g_access_flag_names[] = {"abstract","annotation","bridge","enum",\
                             "final","interface","native","private",\
                             "protected","public","static","strict",\
                             "super","synthetic","varargs"
};
void print_class_constants(const class* c)
{
    ConstantEntry* const_entry = (ConstantEntry*)(c->buffer+c->constant_entry_offset);
    ConstantEntry* this_const_entry;
    for(int i=0;i<c->constant_pool_entry_count;i++) {
        this_const_entry = &const_entry[i];
        printk("#%d:",i);
        switch(this_const_entry->type) {
            case CONSTANT_NULL:{
                printk("null\n");
                break;
            }
            case CONSTANT_UTF8:{
                printk("type=utf-8,val=%s\n", \
                     c->buffer+((UTF8InfoConstant*)this_const_entry)->buffer_offset);
                break;
            }
            case CONSTANT_INT: {
                printk("type=int,val=%d\n",((IntegerInfoConstant*)this_const_entry)->val);
                break;
            }
            case CONSTANT_FLOAT: {
                printk("type=float,val=%f\n",((FloatInfoConstant*)this_const_entry)->val);
                break;
            }
            case CONSTANT_LONG: {
                printk("type=long,val=%lld\n",((LongInfoConstant*)this_const_entry)->val);
                break;
            }
            case CONSTANT_DOUBLE: {
                printk("type=double,val=%lf\n",((DoubleInfoConstant*)this_const_entry)->val);
                break;
            }
            case CONSTANT_CLASS: {
                printk("type=class,name=%s\n",class_get_utf8_string(c,((ClassInfoConstant*)this_const_entry)->name_index));
                break;
            }
            case CONSTANT_STRING: {
                printk("type=string,val=%s\n",class_get_utf8_string(c,((ClassInfoConstant*)this_const_entry)->name_index));
                break;
            }
            case CONSTANT_FIELDREF: {
                printk("type=field,class index=%d,name&type index=%d\n", \
                      ((FieldRefConstant*)this_const_entry)->class_index, \
                      ((FieldRefConstant*)this_const_entry)->name_and_type_index);
                break;
            }
            case CONSTANT_METHODREF: {
                printk("type=method,class index=%d,name&type index=%d\n", \
                      ((MethodRefConstant*)this_const_entry)->class_index, \
                      ((MethodRefConstant*)this_const_entry)->name_and_type_index);
                break;
            }
            case CONSTANT_INTERFACEMETHODREF: {
                printk("type=interface,class index=%d,name&type index=%d\n", \
                      ((InterfaceRefConstant*)this_const_entry)->class_index, \
                      ((InterfaceRefConstant*)this_const_entry)->name_and_type_index);
            }
            case CONSTANT_NAMEANDTYPE: {
                printk("type=name and type,class name=%s,type=%s\n", \
                      class_get_utf8_string(c,((NameAndTypeInfoConstant*)this_const_entry)->name_index), \
                      class_get_utf8_string(c,((NameAndTypeInfoConstant*)this_const_entry)->descriptor_index));
                break;
            }
            default: {
                printk("Unknown type:%d.\n",const_entry[i].type);
            }
        }
    }
}
const U8* class_get_utf8_string(const class* c,int no)
{
    if(no>=c->constant_pool_entry_count) {
        return nullptr;
    }
    UTF8InfoConstant* const_entry = (UTF8InfoConstant*)(c->buffer+c->constant_entry_offset);
    if(const_entry[no].type!=CONSTANT_UTF8) {
        return nullptr;
    }
    return c->buffer+const_entry[no].buffer_offset;
}
const U16 class_get_class_name_index(const class* c,int no)
{
    if(no>=c->constant_pool_entry_count) {
        return FzOS_ERROR;
    }
    ClassInfoConstant* const_entry = (ClassInfoConstant*)(c->buffer+c->constant_entry_offset);
    if(const_entry[no].type!=CONSTANT_CLASS) {
        return FzOS_ERROR;
    }
    return const_entry[no].name_index;
}
U16 class_get_utf8_string_index(const class* c,const U8* name)
{
    UTF8InfoConstant* const_entry = (UTF8InfoConstant*)(c->buffer+c->constant_entry_offset);
    for(int i=0;i<c->constant_pool_entry_count;i++) {
        if(const_entry->type==CONSTANT_UTF8) {
            if(!strcomp((char*)&c->buffer[const_entry->buffer_offset],(char*)name)) {
                return i;
            }
        }
        const_entry++;
    }
    return 0;
}
AttributeInfoEntry* class_get_class_attribute_by_name(const class* c,U16 name_index)
{
    AttributeInfoEntry* a = (AttributeInfoEntry*)&c->buffer[c->class_attributes_entry_offset];
    for(int i=0;i<c->class_attributes_entry_count;i++) {
        if(a->attribute_name_index==name_index) {
            return a;
        }
        a++;
    }
    return nullptr;
}
MethodInfoEntry* class_get_method_by_name_and_desc(const class* c,U16 name_index,U16 desc_index)
{
    MethodInfoEntry* m = (MethodInfoEntry*)&c->buffer[c->method_pool_entry_offset];
    for(int i=0;i<c->method_pool_entry_count;i++) {
        if(m->name_index==name_index&&m->descriptor_index==desc_index) {
            return m;
        }
        m++;
    }
    return nullptr;
}
void print_field_and_method_info(const class* c)
{
    FieldInfoEntry* field_entry = (FieldInfoEntry*)&(c->buffer[c->fields_pool_entry_offset]);
    if(c->fields_pool_entry_count) {
        printk("Fields:\n");
    }
    for(int i=0;i<c->fields_pool_entry_count;i++) {
        printk("#%d:",i);
        for(int j=0;j<15;j++) {
            if(field_entry[i].access_flags&g_access_flags[j]) {
                printk("%s ",g_access_flag_names[j]);
            }
        }
        printk("%s %s,attributes_count:%d\n",\
               class_get_utf8_string(c,field_entry[i].name_index),\
               class_get_utf8_string(c,field_entry[i].descriptor_index),\
               field_entry[i].attribute_count
        );

    }
    print_method_info(c,(MethodInfoEntry*)&(c->buffer[c->method_pool_entry_offset]));

}
void print_method_info(const class* c,const MethodInfoEntry* method_entry)
{
    U16 code_name_index = class_get_utf8_string_index(c,(U8*)"Code");
    if(c->method_pool_entry_count) {
        printk("Methods:\n");
    }
    for(int i=0;i<c->method_pool_entry_count;i++) {
        printk("#%d:",i);
        for(int j=0;j<15;j++) {
            if(method_entry[i].access_flags&g_access_flags[j]) {
                printk("%s ",g_access_flag_names[j]);
            }
        }
        printk("%s %s,attributes_count:%d\n",\
               class_get_utf8_string(c,method_entry[i].name_index),\
               class_get_utf8_string(c,method_entry[i].descriptor_index),\
               method_entry[i].attribute_count
        );
        if(method_entry[i].attribute_count) {
            AttributeInfoEntry* attribute_entry = (AttributeInfoEntry*)&(c->buffer[method_entry[i].attribute_info_entry_offset]);
            for(int j=0;j<method_entry[i].attribute_count;j++) {
                 if(code_name_index!=0&&attribute_entry->attribute_name_index==code_name_index) {
                    CodeAttribute* attr = (CodeAttribute*)&c->buffer[attribute_entry->info_offset];
                    U32 code_count = bswap32(attr->code_length);
                    printk("Code,max stack:%d,max local var:%d,length:%d\n",\
                            bswap16(attr->max_stack),\
                            bswap16(attr->max_locals),\
                            code_count);
                    for(int k=0;k<code_count;k++) {
                        printk("%b ",attr->code[k]);
                    }
                }
                else {
                    printk("%s:length:%d, value:\n",class_get_utf8_string(c,attribute_entry->attribute_name_index),attribute_entry->attribute_length);
                    const U8* val = &c->buffer[attribute_entry->info_offset];
                    for(int k=0;k<attribute_entry->attribute_length;k++) {
                        printk("%b ",*val);
                        val++;
                    }
                }
                printk("\n");
            }
        }
    }
}
void print_class_info(const class* c)
{
    U64 source_file_name_index = class_get_utf8_string_index(c,(U8*)"SourceFile");
    if(source_file_name_index) {
        AttributeInfoEntry* e = class_get_class_attribute_by_name(c,source_file_name_index);
        U16 file_name = bswap16(*(U16*)&(c->buffer[e->info_offset]));
        printk("Source File:%s\n",class_get_utf8_string(c,file_name));
    }
    for(int j=0;j<15;j++) {
        if(c->access_flag&g_access_flags[j]) {
            printk("%s ",g_access_flag_names[j]);
        }
    }
    printk("class %s extends %s ",class_get_utf8_string(c,class_get_class_name_index(c,c->this_class)),
                                 class_get_utf8_string(c,class_get_class_name_index(c,c->super_class)));
    if(c->interface_pool_entry_count) {
        printk("implements ");
        U16* interfaces = (U16*)(&c->buffer[c->interface_pool_entry_offset]);
        for(int j=0;j<c->interface_pool_entry_count;j++) {
            printk("%s ",class_get_utf8_string(c,class_get_class_name_index(c,interfaces[j])));
        }
    }
    printk("\n");

}
AttributeInfoEntry* class_get_method_attribute_by_name(const class* c,const MethodInfoEntry* entry,U16 name_index)
{
    AttributeInfoEntry* a = (AttributeInfoEntry*)&c->buffer[entry->attribute_info_entry_offset];
    for(int i=0;i<c->class_attributes_entry_count;i++) {
        if(a->attribute_name_index==name_index) {
            return a;
        }
        a++;
    }
    return nullptr;
}
inline ConstantEntry* get_const_entry_by_index(class* c,int no)
{
    return ((ConstantEntry*)(c->buffer+c->constant_entry_offset))+no;
}
inline class* get_class_by_index(class* c,int no)
{
    const U8* target_class_name = class_get_utf8_string(c,((ClassInfoConstant*)get_const_entry_by_index(c,no))->name_index);
    return getclass(target_class_name);
}
inline FieldInfoEntry* get_field_by_name_and_type(class* c,const U8* name,const U8* type)
{
    FieldInfoEntry* field_entry = (FieldInfoEntry*)&(c->buffer[c->fields_pool_entry_offset]);
    if(c->fields_pool_entry_count) {
        for(int i=0;i<c->fields_pool_entry_count;i++) {
            if(!strcomp((char*)name,(char*)class_get_utf8_string(c,field_entry[i].name_index))
             &&!strcomp((char*)type,(char*)class_get_utf8_string(c,field_entry[i].descriptor_index))) {
                return field_entry;
            }
        }
    }
    return nullptr;
}
inline MethodInfoEntry* get_method_by_name_and_type(class* c,const U8* name,const U8* type)
{
    MethodInfoEntry* method_entry = (MethodInfoEntry*)&(c->buffer[c->method_pool_entry_offset]);
    if(c->method_pool_entry_count) {
        for(int i=0;i<c->method_pool_entry_count;i++) {
            if(!strcomp((char*)name,(char*)class_get_utf8_string(c,method_entry[i].name_index))
             &&!strcomp((char*)type,(char*)class_get_utf8_string(c,method_entry[i].descriptor_index))) {
                return method_entry;
            }
        }
    }
    return nullptr;
}
