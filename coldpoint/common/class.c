#include <coldpoint/common/class.h>
#include <common/printk.h>
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
