//Simple Wav File Handler.
#ifndef WAV_H
#define WAV_H
#include <types.h>
typedef struct {
    U32 sample_rate;
    U8  sample_depth;
    U8  channels;
    U32 offset_to_data;
    U32 data_length;
} AudioInfo;
typedef struct {
    U8  id[4];
    U32 file_size;
    U8  type[4];
} __attribute__((packed)) WavFileHeader;
typedef struct {
    U8 id[4];
    U32 size;
    U16 audio_format;
    U16 num_channels;
    U32 sample_rate;
    U32 byte_rate;
    U16 block_align;
    U16 bits_per_sample;
}__attribute__((packed)) WavFormatChunk;
typedef struct {
    U8  id[4];
    U32 size;
    U8  data[0];
}__attribute__((packed)) WavDataChunk;
int stat_wav(AudioInfo* info_buffer,void* file_buffer,U64 filesize);
struct HDAConnector;
int play_wav(AudioInfo* info_buffer,void* file_buffer,struct HDAConnector* connector);
#endif
