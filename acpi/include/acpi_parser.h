#ifndef ACPI_PARSER
#define ACPI_PARSER
#include <types.h>
//定义ACPI RSDP指针文件结构。
typedef struct {
    char Signature[8];
    U8 Checksum;
    char OEMID[6];
    U8 Revision;
    U32 RsdtAddress;
} __attribute__ ((packed)) RSDPDescriptor;
typedef struct {
    RSDPDescriptor firstPart;
    U32 Length;
    U64 XsdtAddress;
    U8 ExtendedChecksum;
    U8 reserved[3];
} __attribute__ ((packed)) RSDPDescriptor20;
typedef struct {
    char Signature[4];
    U32 Length;
    U8 Revision;
    U8 Checksum;
    char OEMID[6];
    char OEMTableID[8];
    U32 OEMRevision;
    U32 CreatorID;
    U32 CreatorRevision;
}__attribute__ ((packed)) ACPISDTHeader;
#endif
void parse_acpi(U8* in);
void* get_xsdt_addr(RSDPDescriptor20* rsdp);