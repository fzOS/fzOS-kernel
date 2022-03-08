#include <coldpoint/native/nativehandler.h>
#include <common/kstring.h>
#include <coldpoint/native/native_iostream.h>
const char* const g_native_class_names[] = {
    g_iostream_class_name
};
NativeClassEntry* const g_native_class_entries[] = {
    iostream_class_entry
};
static const I64 g_native_class_count = sizeof(g_native_class_names)/sizeof(const char*);
_Static_assert(sizeof(g_native_class_names)==sizeof(g_native_class_entries),"Mismatch size between name and entries!");
NativeClassEntry* const get_native_class_entry(const U8* name)
{
    for(I64 i=0;i<g_native_class_count;i++) {
        if(!strcomp((const char*)name,g_native_class_names[i])) {
            return (NativeClassEntry*)((U64)g_native_class_entries[i] | KERNEL_ADDR_OFFSET);
        }
    }
    return nullptr;
}
