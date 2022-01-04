#ifndef HDA_H
#define HDA_H
#include <drivers/blockdev.h>
#include <drivers/pci.h>
#define MAX_CODEC_COUNT 15
#define MAX_AFG_COUNT 4
typedef volatile struct
{
    U16 gcap;
    U8  vmin;
    U8  vmaj;
    U16 outpay;
    U16 inpay;
    U32 gctl;
    U16 wakeen;
    U16 statests;
    U16 gsts;
    U8  reserved1[6];
    U16 outstrmpay;
    U16 instrmpay;
    U8  reserved2[4];
    U32 intctl;
    U32 intsts;
    U8  reserved3[8];
    U32 walclk;
    U8  reserved4[4];
    U32 ssync;
    U8  reserved5[4];
    U32 corblbase;
    U32 corbubase;
    U16 corbwp;
    U16 corbrp;
    U8  corbctl;
    U8  corbsts;
    U8  corbsize;
    U8  reserved6;
    U32 rirblbase;
    U32 rirbubase;
    U16 rirbwp;
    U16 rintcnt;
    U8  rirbctl;
    U8  rirbsts;
    U8  rirbsize;
    U8  reserved7;
    U32 icoi;
    U32 icii;
    U16 icis;
    U8  reserved8[6];
    U32 dpiblbase;
    U32 dpibubase;
    U8  reserved9[8];
} __attribute__((packed)) HDABaseRegisters;
typedef union {
    U32 raw;
    struct {
        int chan_count_lsb:1;
        int in_amp_present:1;
        int out_amp_present:1;
        int amp_param_override:1;
        int format_override:1;
        int stripe:1;
        int proc_widget:1;
        int unsol_capable:1;
        int conn_lost:1;
        int digital:1;
        int power_cntrl:1;
        int l_r_swap:1;
        int cp_caps:1;
        int chan_count_ext:3;
        int delay:4;
        int type:4;
        int reserved:8;
    } __attribute__((packed)) split;
} AudioWidgetCap;
typedef volatile struct
{
    U8  sdctl[3];
    U8  sdsts;
    U32 sdlpib;
    U32 sdcbl;
    U16 sdlvi;
    U16 reserved1[2];
    U16 sdfifod;
    U16 sdfmt;
    U8  reserved2[4];
    U32 sdbdpl;
    U32 sdbdpu;
} __attribute__((packed)) StreamDescRegisters;
struct HDACodec;
typedef struct
{
    block_dev dev;
    PCIDevice base;
    struct HDACodec* codecs[MAX_CODEC_COUNT];
    U8 afg_nodes[MAX_AFG_COUNT];//Static storage of AFG tree.
    HDABaseRegisters* registers;
    U32* corb;
    U64* rirb;
    U32 corb_count,rirb_count;
} HDAController;
typedef struct HDACodec
{
    int codec_id;
    HDAController* controller;
    U8 audio_widget_count;
    U8 afg_id;
    //Output,Input,Mixer,Selector,Pin Complex,Power Widget
    U8 output_index;
    U8 input_index;
    U8 mixer_index;
    U8 selector_index;
    U8 pin_complex_index;
    U8 power_widget_index;
    U8 audio_widgets[0];
} HDACodec;

typedef enum {
    CODEC_GET_PARAMETER=0xf00,
    CODEC_GET_SELECTED_INPUT=0xf01,
    CODEC_SET_SELECTED_INPUT=0x701,
    CODEC_GET_STREAM_CHANNEL=0xf06,
    CODEC_SET_STREAM_CHANNEL=0x706,
    CODEC_GET_PIN_WIDGET_CONTROL=0xf07,
    CODEC_SET_PIN_WIDGET_CONTROL=0x707,
    CODEC_GET_VOLUME_CONTROL=0xf0f,
    CODEC_SET_VOLUME_CONTROL=0x70f,
    CODEC_GET_CONF_DEFAULT=0xf1c,
    CODEC_GET_CONV_CHAN_CNT=0xf2d,
    CODEC_SET_CONV_CHAN_CNT=0x72d,
    CODEC_FUNCTION_RESET=0x7ff
} HDACommand;
typedef enum {
    PARAM_VEN_DEV_ID=0x00,
    PARAM_REV_ID=0x02,
    PARAM_NODE_COUNT=0x04,
    PARAM_FUNC_GROUP_TYPE=0x05,
    PARAM_AUDIO_GROUP_CAP=0x08,
    PARAM_AUDIO_WIDGET_CAP=0x09,
    PARAM_SUPPORTED_PCM_RATES=0x0a,
    PARAM_SUPPORTED_FORMATS=0x0b,
    PARAM_PIN_CAP=0x0c,
    PARAM_INPUT_AMP_CAP=0x0d,
    PARAM_OUTPUT_AMP_CAP=0x12,
    PARAM_CON_LOST_LENGTH=0x0e,
    PARAM_SUPPORTED_PWR_STATES=0x0f,
    PARAM_PROCESSING_CAP=0x10,
    PARAM_GPIO_COUNT=0x11,
    PARAM_VOLUME_CAP=0x13
} HDAParam;
typedef union {
    struct {
        int data:8;
        HDACommand command:12;
        int node_id:8;
        int codec_addr:4;
    } __attribute__((packed))  split;
    U32 packed;
} HDAVerb;
typedef struct
{
    device_tree_node header;
    HDAController controller;
}HDAControllerTreeNode;
typedef struct
{
    device_tree_node header;
    HDACodec codec;
}HDACodecTreeNode;
void hda_register(U8 bus,U8 slot,U8 func);

#endif
