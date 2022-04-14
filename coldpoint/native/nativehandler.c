#include <coldpoint/native/nativehandler.h>
#include <common/kstring.h>
#include <coldpoint/native/native_iostream.h>
#include <coldpoint/native/native_typewrapper.h>
#include <coldpoint/native/native_file.h>
#include <coldpoint/native/native_audiomanager.h>
#include <coldpoint/native/native_windowmanager.h>
#include <coldpoint/native/native_thread.h>
NativeClassRegister* const g_native_class_register_handlers[] = {
    iostream_class_register,
    typewrapper_class_register,
    file_class_register,
    audiomanager_class_register,
    windowmanager_class_register,
    thread_class_register
};
const I64 g_native_class_count = sizeof(g_native_class_register_handlers)/sizeof(NativeClassRegister*);
cpstatus register_native_classes(InlineLinkedList* loaded_class_list)
{
    cpstatus status = COLD_POINT_SUCCESS;
    for(I64 i=0;i<g_native_class_count;i++) {
        status = g_native_class_register_handlers[i](loaded_class_list);
        if(status!=COLD_POINT_SUCCESS) {
            break;
        }
    }
    return status;
}
