#include <dsdt.h>
int parse_dsdt(void* in)
{
    if(validate_table((U8*)in)) {
        printk(" Error:broken dsdt.\n");
        return -1;
    }
    return 0;
}