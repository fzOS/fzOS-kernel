#include <coldpoint/native/native_audiomanager.h>
#include <coldpoint/native/native_file.h>
#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/classloader.h>
#include <common/kstring.h>
#include <memory/memory.h>
#include <drivers/hda.h>
const char * const g_audiomanager_class_name = "fzos/audio/AudioManager";
const char * const g_audio_class_name = "fzos/audio/Audio";
cpstatus native_audiomanager_open_audio(thread* t);
cpstatus native_audiomanager_play_audio(thread* t);
static HDAConnector* g_default_playback_connector;
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
    FileObject *o = (FileObject*)t->stack[t->rsp--].data;
    Array* data_array = allocate_heap(sizeof(Array)+o->f.size);
    data_array->type = (const U8*)"B";
    data_array->length = o->f.size;
    o->f.filesystem->read(&o->f,data_array->value,o->f.size);
    AudioInfo info;
    stat_wav(&info,data_array->value,o->f.size);
    class* c = getclass((const U8*)g_audio_class_name);
    object* obj = new_object(c);
    set_parameter_in_object(obj,"sampleRate","I",g_audio_class_name,info.sample_rate);
    set_parameter_in_object(obj,"channels","I",g_audio_class_name,info.channels);
    set_parameter_in_object(obj,"sampleDepth","I",g_audio_class_name,info.sample_depth);
    set_parameter_in_object(obj,"offsetToPcm","I",g_audio_class_name,info.offset_to_data);
    set_parameter_in_object(obj,"dataLength","I",g_audio_class_name,info.data_length);
    set_parameter_in_object(obj,"data","[B",g_audio_class_name,(U64)data_array);
    t->stack[++t->rsp].data = (U64)obj;
    t->stack[t->rbp].type   = STACK_TYPE_REFERENCE;
    return COLD_POINT_SUCCESS;
}
cpstatus native_audiomanager_play_audio(thread* t)
{
    if(g_default_playback_connector!=nullptr) {
        object* obj = (object*)t->stack[t->rsp--].data;
        AudioInfo info;
        info.sample_rate    = get_parameter_in_object(obj,"sampleRate","I",g_audio_class_name);
        info.channels       = get_parameter_in_object(obj,"channels","I",g_audio_class_name);
        info.sample_depth   = get_parameter_in_object(obj,"sampleDepth","I",g_audio_class_name);
        info.offset_to_data = get_parameter_in_object(obj,"offsetToPcm","I",g_audio_class_name);
        info.data_length    = get_parameter_in_object(obj,"dataLength","I",g_audio_class_name);
        Array* data         = (Array*)get_parameter_in_object(obj,"data","[B",g_audio_class_name);
        t->sem = play_pcm(&info,((void*)data->value)+info.offset_to_data,g_default_playback_connector);
        t->status = THREAD_BLOCKED;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus audiomanager_class_entry(thread* t,const U8* name,const U8* desc,NativeClassOperations operations)
{
    switch (operations) {
        case NATIVE_GETSTATIC: case NATIVE_PUTSTATIC: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_GETFIELD: case NATIVE_PUTFIELD: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_INVOKE: {
            for(U64 i=0;i<sizeof(g_audiomanager_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_audiomanager_methods[i].name)
                && !strcomp((char*)desc,(char*)g_audiomanager_methods[i].desc)) {
                    return g_audiomanager_methods[i].method(t);
                }
            }
            except(t,"Native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_NEW: {
            return COLD_POINT_NOT_IMPLEMENTED;
        }
    }
    return COLD_POINT_SUCCESS;
}

cpstatus audiomanager_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_audiomanager_class_linked_node.node,-1);
    file file;
    int ret = generic_open("/Audio.class",&file);
    if(ret !=FzOS_SUCCESS) {
        printk(" Open fzos.audio.Audio fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    void* buf = memalloc(file.size);
    ret = file.filesystem->read(&file,buf,file.size);
    if(ret==0) {
        memfree(buf);
        printk(" Read Audio fail: %d!\n",ret);
        return COLD_POINT_EXEC_FAILURE;
    }
    loadclass(buf);
    memfree(buf);
    HDACodecTreeNode* hda_codec_node = (HDACodecTreeNode*)device_tree_resolve_by_path("/Devices/HDAController0/HDACodec0",nullptr,DT_RETURN_IF_NONEXIST);
    if(hda_codec_node!=nullptr) {
        g_default_playback_connector = hda_codec_node->codec.default_output;
    }
    return COLD_POINT_SUCCESS;
}
