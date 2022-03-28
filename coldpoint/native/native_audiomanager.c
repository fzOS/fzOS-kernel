#include <coldpoint/native/native_audiomanager.h>
const char * const g_audiomanager_class_name = "fzos/audio/AudioManager";
cpstatus native_audiomanager_open_audio(thread* t);
cpstatus native_audiomanager_play_audio(thread* t);
static NativeMethod g_audiomanager_methods[] = {
    {
        (U8*)"openAudioFromFile",
        (U8*)"(Lfzos/util/File;)Lfzos/audio/Audio;",
        native_audiomanager_open_audio
    },
    {
        (U8*)"playAudio",
        (U8*)"(Lfzos/audio/Audio;)V",
        native_audiomanager_play_audio
    }
};
static NativeClassInlineLinkedListNode g_audiomanager_class_linked_node = {
    .c.class_name = (const U8*)g_audiomanager_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = audiomanager_class_entry
};
cpstatus native_audiomanager_open_audio(thread* t)
{

    return COLD_POINT_SUCCESS;
}
cpstatus native_audiomanager_play_audio(thread* t)
{
    return COLD_POINT_SUCCESS;
}
cpstatus audiomanager_class_entry(thread* t,const U8* name,const U8* desc,NativeClassOperations operations)
{
    (void)g_audiomanager_methods;
    return COLD_POINT_SUCCESS;
}

cpstatus audiomanager_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_audiomanager_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
