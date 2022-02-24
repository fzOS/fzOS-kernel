#include <coldpoint/heap/heap.h>
#include <common/printk.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <memory/memory.h>
#define OBJECT_CHUNK_SIZE 512
object* new_object(class* c)
{
    object* o = memalloc(OBJECT_CHUNK_SIZE);
    U64 object_size = sizeof(object);
    U64 allocated_size = OBJECT_CHUNK_SIZE;
    U64 current_index=0;
    class* current = c;
    while(1) {
        FieldInfoEntry *field_entry = (FieldInfoEntry *)&(c->buffer[c->fields_pool_entry_offset]);
        for(int i=0;i<current->fields_pool_entry_count;i++) {
            if(object_size+sizeof(object)>allocated_size) {
                o = memrealloc(o,allocated_size+OBJECT_CHUNK_SIZE);
                allocated_size += OBJECT_CHUNK_SIZE;
            }
            o->var[current_index].signature = (char*)class_get_utf8_string(c,field_entry[i].name_index);
            o->var[current_index].typename  = (char*)class_get_utf8_string(c,field_entry[i].descriptor_index);
            o->var[current_index].class     = (char*)c->class_name;
            current_index++;
            object_size += sizeof(ObjectVar);
        }
        MethodInfoEntry* method_entry = (MethodInfoEntry *)&(c->buffer[c->method_pool_entry_offset]);
        for(int i=0;i<current->method_pool_entry_count;i++) {
            if(object_size+sizeof(object)>allocated_size) {
                o = memrealloc(o,allocated_size+OBJECT_CHUNK_SIZE);
                allocated_size += OBJECT_CHUNK_SIZE;
            }
            o->var[current_index].signature = (char*)class_get_utf8_string(c,method_entry[i].name_index);
            o->var[current_index].typename  = (char*)class_get_utf8_string(c,method_entry[i].descriptor_index);
            o->var[current_index].class     = (char*)c->class_name;
            current_index++;
            object_size += sizeof(ObjectVar);
        }
        current = getclass(class_get_utf8_string(c,class_get_class_name_index(c,c->super_class)));
        if(!strcomp((char*)c->class_name,"java/lang/Object")) {
            break;
        }
    }
    return o;
}
