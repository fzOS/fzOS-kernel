#ifndef APIC_H
#define APIC_H
#include <acpi/acpi_parser.h>
int parse_apic(U8* in);
typedef struct {
    ACPISDTHeader Common;
    U32 LocalAPICAddress;
    U32 Flags;
}__attribute__ ((packed)) APICHeader;
typedef struct {
    U8 Type;
    U8 Length;
    U8 ProcesserUID;
    U8 APICID;
    U32 Flags;
} __attribute__ ((packed)) ProcessorLocalApic;
typedef struct {
    U8 Type;
    U8 Length;
    U8 IOACPIID;
    U8 Reserved;
    U32 IOAPICAddr;
    U32 GlobalSystemInterruptBase;
} __attribute__ ((packed)) IOApic;
#endif
