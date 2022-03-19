#include <acpi/hpet_table.h>
#include <drivers/hpet.h>
#include <common/printk.h>
int parse_hpet(void* in)
{
    if(validate_table((U8*)in)) {
        printk(" Error:broken hpet.\n");
        return -1;
    }
    HPETHeader* header = (HPETHeader*)in;
    g_hpet_base_address = (HPETResgister*)header->BaseAddress;
    return 0;
}
