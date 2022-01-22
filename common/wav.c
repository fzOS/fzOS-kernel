#include <common/wav.h>
#include <memory/memory.h>
#include <common/printk.h>
#include <drivers/hda.h>
int stat_wav(AudioInfo* info_buffer,void* file_buffer,U64 filesize)
{
    //first,check WAV file header.
    WavFileHeader* header = (WavFileHeader*) file_buffer;
    if(memcmp(header->id,"RIFF",4)) {
        return FzOS_INVALID_INPUT;
    }
    if(memcmp(header->type,"WAVE",4)) {
        return FzOS_INVALID_INPUT;
    }
    byte* p = file_buffer+sizeof(WavFileHeader);
    byte* p_eof = file_buffer + filesize;
    //get "fmt " section.
    while(*p!='f'||memcmp(p,"fmt ",4)) {
        p++;
        if(p==p_eof) {
            return FzOS_INVALID_INPUT;
        }
    }
    WavFormatChunk* format = (WavFormatChunk*) p;
    if(format->audio_format!=0x01) { //We only support PCM.
        return FzOS_INVALID_INPUT;
    }
    info_buffer->channels = format->num_channels;
    info_buffer->sample_rate = format->sample_rate;
    info_buffer->sample_depth = format->bits_per_sample;
    p += sizeof(WavFormatChunk);
    //get "data" section.
    while(*p!='d'||memcmp(p,"data",4)) {
        p++;
        if(p==p_eof) {
            return FzOS_INVALID_INPUT;
        }
    }
    WavDataChunk* data = (WavDataChunk*) p;
    info_buffer->data_length = data->size;
    info_buffer->offset_to_data = (void*)data->data-file_buffer;
    return FzOS_SUCCESS;
}
int play_wav(AudioInfo* info_buffer,void* file_buffer,HDAConnector* connector)
{
    semaphore* sem = play_pcm(info_buffer,file_buffer+info_buffer->offset_to_data,connector);
    acquire_semaphore(sem);
    return FzOS_SUCCESS;
}
