#include <coldpoint/common/class.h>
#include <common/printk.h>
int access_flags[] = {ACCESS_ABSTRACT,ACCESS_ANNOTATION,ACCESS_BRIDGE,ACCESS_ENUM,\
                      ACCESS_FINAL,ACCESS_INTERFACE,ACCESS_NATIVE,ACCESS_PRIVATE,\
                      ACCESS_PROTECTED,ACCESS_PUBLIC,ACCESS_STATIC,ACCESS_STRICT,\
                      ACCESS_SUPER,ACCESS_SYNTHETIC,ACCESS_VARARGS};
char* access_flag_names[] = {"abstract","annotation","bridge","enum",\
                             "final","interface","native","private",\
                             "protected","public","static","strict",\
                             "super","synthetic","varargs"
};
void print_class_constants(const class* c)
{
    constant_entry* const_entry = (constant_entry*)(c->buffer+c->constant_entry_offset);
    constant_entry* this_const_entry;
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
void print_field_and_method_info(const class* c)
{
    field_info_entry* field_entry = (field_info_entry*)&(c->buffer[c->fields_pool_entry_offset]);
    if(c->fields_pool_entry_count) {
        printk("Fields:\n");
    }
    for(int i=0;i<c->fields_pool_entry_count;i++) {
        printk("#%d:",i);
        for(int j=0;j<15;j++) {
            if(field_entry[i].access_flags&access_flags[j]) {
                printk("%s ",access_flag_names[j]);
            }
        }
        printk("%s %s,attributes_count:%d\n",\
               class_get_utf8_string(c,field_entry[i].name_index),\
               class_get_utf8_string(c,field_entry[i].descriptor_index),\
               field_entry[i].attribute_count
        );

    }
    method_info_entry* method_entry = (method_info_entry*)&(c->buffer[c->method_pool_entry_offset]);
    if(c->method_pool_entry_count) {
        printk("Methods:\n");
    }
    for(int i=0;i<c->method_pool_entry_count;i++) {
        printk("#%d:",i);
        for(int j=0;j<15;j++) {
            if(method_entry[i].access_flags&access_flags[j]) {
                printk("%s ",access_flag_names[j]);
            }
        }

        printk("%s %s,attributes_count:%d\n",\
               class_get_utf8_string(c,method_entry[i].name_index),\
               class_get_utf8_string(c,method_entry[i].descriptor_index),\
               method_entry[i].attribute_count
        );
        if(method_entry[i].attribute_count) {
            attribute_info_entry* attribute_entry = (attribute_info_entry*)&(c->buffer[method_entry[i].attribute_info_entry_offset]);
            for(int j=0;j<method_entry[i].attribute_count;j++) {
                printk("%s:length:%d, value:\n",class_get_utf8_string(c,attribute_entry->attribute_name_index),attribute_entry->attribute_length);
                const U8* val = &c->buffer[attribute_entry->info_offset];
                for(int k=0;k<attribute_entry->attribute_length;k++) {
                    printk("%b ",*val);
                    val++;
                }
                printk("\n");
            }
        }
    }
}
void print_class_info(const class* c)
{
    for(int j=0;j<15;j++) {
        if(c->access_flag&access_flags[j]) {
            printk("%s ",access_flag_names[j]);
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
