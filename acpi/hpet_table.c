#include <acpi/hpet_table.h>
#include <common/printk.h>
int parse_hpet(void* in)
{
    if(validate_table((U8*)in)) {
        printk(" Error:broken hpet.\n");
        return -1;
    }
    HPETHeader* header = (HPETHeader*)in;
    printk("APIC Address:0x%x,Clock period:%d cycles.\n",header->BaseAddress,header->CountTick);
    return 0;
}
