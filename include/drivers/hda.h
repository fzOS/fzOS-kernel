#ifndef HDA_H
#define HDA_H
#include <drivers/blockdev.h>
#include <drivers/pci.h>
typedef volatile struct {
    U16 gcap;
    U8  vmin;
    U8  vmaj;
    U16 outpay;
    U16 inpay;
    U32 gctl;
    U16 wakeen;
    U16 wakests;
    U16 gsts;

} __attribute__((packed)) HDARegisters;
typedef struct
{
    block_dev dev;
    PCIDevice base;
    HDARegisters* registers;
} HDAController;
void hda_register(U8 bus,U8 slot,U8 func);

#endif
