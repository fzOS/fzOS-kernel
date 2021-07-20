#include <acpi/fadt.h>
#include <common/printk.h>
#include <common/io.h>
#include <acpi/dsdt.h>
#include <lai/helpers/sci.h>
U8 acpi_interrupt;
int parse_fadt(void* in) {
    if(validate_table((U8*)in)) {
        printk(" Error:broken fadt.\n");
        return -1;
    }
    FADT* fadt = (FADT*)in;
    acpi_table_entries[4] = (void*)(fadt->X_Dsdt|KERNEL_ADDR_OFFSET);
    if(fadt->SMI_CommandPort) {
        if(fadt->AcpiEnable||fadt->AcpiDisable) {
            debug(" Enabling ACPI.\n");
            outb(fadt->SMI_CommandPort,fadt->AcpiEnable);
        }
    }
    acpi_interrupt = fadt->SCI_Interrupt;
    acpi_table_entries[4] = (void*)(fadt->X_Dsdt|KERNEL_ADDR_OFFSET);
    return parse_dsdt(acpi_table_entries[4]);
}
void acpi_enable_power_button(void)
{
    FADT* fadt = (FADT*)(acpi_table_entries[0]);
    //获取pm1(a/b)_enable_registers.
    U16 pm1_enable_port = (U16)fadt->X_PM1aEventBlock.Address+fadt->PM1EventLength/2;
    //printk("%x\n",pm1_enable_port+fadt->PM1EventLength/2);
    outw(pm1_enable_port,1<<8);
    if(pm1_enable_port=(U16)fadt->X_PM1bEventBlock.Address,pm1_enable_port) {
        pm1_enable_port += fadt->PM1EventLength/2;
        outw(pm1_enable_port,1<<8);
    }
}
