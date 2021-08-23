#include <common/power_control.h>
#include <acpi/acpi_parser.h>
#include <acpi/fadt.h>
#include <filesystem/filesystem.h>
void poweroff(void)
{
    unmount_all_mounted_filesystems();
    FADT* fadt = (FADT*)(acpi_table_entries[0]);
    outw(fadt->X_PM1aControlBlock.Address,0b11010000000000); //FIXME:Get real val from DSDT.
}
void reset(void)
{
    FADT* fadt = (FADT*)(acpi_table_entries[0]);
    outb((fadt->ResetReg.Address),fadt->ResetValue);
}
