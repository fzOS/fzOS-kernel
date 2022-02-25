#include <coldpoint/heap/heap.h>
#include <common/printk.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <memory/memory.h>
#include <common/bswap.h>
#define OBJECT_CHUNK_SIZE 512
object* new_object(class* c)
{
    object* o = memalloc(OBJECT_CHUNK_SIZE);
    U64 object_size = sizeof(object);
    U64 allocated_size = OBJECT_CHUNK_SIZE;
    U64 current_index=0;
    class* current = c;
    while(1) {
        U64 code_name_index = class_get_utf8_string_index(c,(U8*)"Code");
        FieldInfoEntry *field_entry = (FieldInfoEntry *)&(current->buffer[current->fields_pool_entry_offset]);
        for(int i=0;i<current->fields_pool_entry_count;i++) {
            //Skip static.
            if(field_entry[i].access_flags&ACCESS_STATIC) {
                continue;
            }
            if(object_size+sizeof(object)>allocated_size) {
                o = memrealloc(o,allocated_size+OBJECT_CHUNK_SIZE);
                allocated_size += OBJECT_CHUNK_SIZE;
            }
            o->var[current_index].signature = (char*)class_get_utf8_string(current,field_entry[i].name_index);
            o->var[current_index].typename  = (char*)class_get_utf8_string(current,field_entry[i].descriptor_index);
            o->var[current_index].class     = (char*)current->class_name;
            current_index++;
            object_size += sizeof(ObjectVar);
        }
        MethodInfoEntry* method_entry = (MethodInfoEntry *)&(current->buffer[current->method_pool_entry_offset]);
        for(int i=0;i<current->method_pool_entry_count;i++) {
            if(object_size+sizeof(object)>allocated_size) {
                o = memrealloc(o,allocated_size+OBJECT_CHUNK_SIZE);
                allocated_size += OBJECT_CHUNK_SIZE;
            }
            if(method_entry[i].attribute_count) {
                AttributeInfoEntry* attribute_entry = (AttributeInfoEntry*)&(c->buffer[method_entry[i].attribute_info_entry_offset]);
                for(int j=0;j<method_entry[i].attribute_count;j++) {
                    if(code_name_index!=0&&attribute_entry->attribute_name_index==code_name_index) {
                        CodeAttribute* attr = (CodeAttribute*)&c->buffer[attribute_entry->info_offset];
                        o->var[current_index].value = (U64)attr;
                    }
                }
            }
            o->var[current_index].signature = (char*)class_get_utf8_string(current,method_entry[i].name_index);
            o->var[current_index].typename  = (char*)class_get_utf8_string(current,method_entry[i].descriptor_index);
            o->var[current_index].class     = (char*)current->class_name;
            current_index++;
            object_size += sizeof(ObjectVar);
        }
        current = getclass(class_get_utf8_string(current,class_get_class_name_index(current,current->super_class)));
        if(current == nullptr) {
            break;
        }
    }
    o->var_count = current_index;
    return o;
}
