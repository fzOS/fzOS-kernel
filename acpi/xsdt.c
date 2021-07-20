#include <acpi/xsdt.h>
#include <acpi/apic.h>
#include <acpi/fadt.h>
#include <common/printk.h>
#include <common/kstring.h>
int parse_xsdt(void* in) {
    //检测表。
    if(validate_table((U8*)in)) {
        printk(" Error:broken xsdt.\n");
        return -1;
    }
    char tableid[9];//For print ACPI Table ID.
    char sig[5]; //For print ACPI Signnature ID.
    //识别头。
    XSDT* xsdt = (XSDT*)in;
    int entry_count = xsdt->header.Length = sizeof(xsdt->header) / sizeof(U64);
    printk(" ACPI Tables:\n");
    U64* entries = (&xsdt->PointerToOtherSDT);
    for(int i=0;i<entry_count;i++) {
        ACPISDTHeader* current_header = (ACPISDTHeader*)entries[i];
        strcopy(tableid,current_header->OEMTableID,8);
        strcopy(sig,current_header->Signature,4);
        printk(" Entry #%d:%x %s %s\n",i,(entries[i]|KERNEL_ADDR_OFFSET),tableid,sig);
        for(int j=0;j<table_count;j++) {
            if(!strcomp(acpi_table_names[j],sig)) {
                acpi_table_entries[j] = (U8*)(entries[i]|KERNEL_ADDR_OFFSET);
                break;
            }
        }
    }
    return (parse_fadt(acpi_table_entries[0])|parse_apic(acpi_table_entries[1]));
}
void* acpi_get_table_by_name(char* name) 
{
    for(int j=0;j<table_count;j++) {
        if(!strcomp(acpi_table_names[j],name)) {
            return acpi_table_entries[j];
        }
    }
    return nullptr;
}
