#ifndef HPET_TABLE_H
#define HPET_TABLE_H
#include <acpi/acpi_parser.h>
typedef struct {
    ACPISDTHeader Common;
    U32 EventTimerBlockID;
    U8  BaseAddressSpaceID;
    U8  BaseAddressBitWidth;
    U8  BaseAddressBitOffset;
    U8  padding;
    U64 BaseAddress;
    U8  HPETNumber;
    U16 CountTick;
    U8  OEMAttributes;
}__attribute__ ((packed)) HPETHeader;
int parse_hpet(void* in);
#endif
