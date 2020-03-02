#ifndef XSDT_H
#define XSDT_H
#include <acpi_parser.h>
typedef struct {
    ACPISDTHeader header;
    U64 PointerToOtherSDT;
} __attribute__ ((packed)) XSDT;
void parse_xsdt(void* in);
#endif