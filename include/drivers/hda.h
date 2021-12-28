#ifndef HDA_H
#define HDA_H
#include <drivers/blockdev.h>
#include <drivers/pci.h>
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
typedef struct
{
    block_dev dev;
    PCIDevice base;
    HDABaseRegisters* registers;
} HDAController;
typedef struct
{
    HDAController* controller;
} HDACodec;
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
