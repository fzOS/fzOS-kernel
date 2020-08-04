#include <fadt.h>
#include <io.h>
#include <dsdt.h>
int parse_fadt(void* in) {
    if(validate_table((U8*)in)) {
        printk(" Error:broken fadt.\n");
        return -1;
    }
    FADT* fadt = (FADT*)in;
    if(fadt->SMI_CommandPort) {
        if(fadt->AcpiEnable||fadt->AcpiDisable) {
            debug(" Enabling ACPI.\n");
            outb(fadt->SMI_CommandPort,fadt->AcpiEnable);
        }
    }
    acpi_table_entries[4] = (void*)fadt->X_Dsdt;
    return parse_dsdt(acpi_table_entries[4]);
}