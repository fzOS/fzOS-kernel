#include <common/wav.h>
#include <memory/memory.h>
#include <common/printk.h>
int stat_wav(WavAudioInfo* info_buffer,void* file_buffer,U64 filesize)
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
int play_wav(WavAudioInfo* info_buffer,void* file_buffer,HDAConnector* connector)
{
    //Creates the PCM buffer.
    void* pcm_buffer = allocate_page((info_buffer->data_length-1)/PAGE_SIZE+1);
    memcpy(pcm_buffer,file_buffer+info_buffer->offset_to_data,info_buffer->data_length);
    PCMFormatStructure s;
    //Creates the BDL Buffer.
    //FIXME:SHOULD NOT ALWAYS USE 0!
    HDABufferDescriptor* desc = connector->codec->controller->buffer_desciptor_list;
    desc->length = info_buffer->data_length;
    desc->address = (U64)pcm_buffer&(~KERNEL_ADDR_OFFSET);
    desc->interrupt_on_completion = 0;
    int stream_no;

    //Get Stream Descriptor.
    StreamDescRegisters* reg = get_output_stream_desc(connector->codec->controller,&stream_no);
    //Fill in basic info.
    reg->sdcbl = info_buffer->data_length;
    reg->sdlvi = 1;
    s.raw = 0;
    s.split.sample_base_rate = (info_buffer->sample_rate==44100?1:0);
    switch(info_buffer->sample_depth) {
        case 8: {s.split.bits_per_sample = 0;break;}
        case 16: {s.split.bits_per_sample = 1;break;}
        case 32: {s.split.bits_per_sample = 0b100;break;}
        default:
            return FzOS_INVALID_INPUT;
    }
    reg->sdfmt = s.raw;
    reg->sdbdpl = (U64)(connector->codec->controller->buffer_desciptor_list) & 0xFFFFFFFF;
    reg->sdbdpu = ((U64)(connector->codec->controller->buffer_desciptor_list) & ~KERNEL_ADDR_OFFSET)>>32;

    int ret = bind_stream_to_converter(connector->codec,stream_no,connector->connected_converter_id);
    //FIXME:Get connected DAC id when not detected.
    if(ret!=FzOS_SUCCESS) {
        return FzOS_ERROR;
    }
    return FzOS_SUCCESS;
}
