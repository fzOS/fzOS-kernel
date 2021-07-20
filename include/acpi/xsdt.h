#ifndef XSDT_H
#define XSDT_H
#include <acpi/acpi_parser.h>
typedef struct {
    ACPISDTHeader header;
    U64 PointerToOtherSDT;
} __attribute__ ((packed)) XSDT;
int parse_xsdt(void* in);
void* acpi_get_table_by_name(char* name);
#endif
