#include <coldpoint/heap/heap.h>
#include <common/printk.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
void new_object(class* c)
{
    int field_count=c->fields_pool_entry_count,method_count=c->method_pool_entry_count;
    class* current = c;
    while(strcomp((char*)(current->class_name),"java/lang/Object")) {
        current = getclass(class_get_utf8_string(c,class_get_class_name_index(c,c->super_class)));
        field_count=current->fields_pool_entry_count;
        method_count=current->method_pool_entry_count;
    }
    printk("Total field count:%d,method count:%d.\n",field_count,method_count);
}
