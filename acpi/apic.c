#include <apic.h>
#include <kerneldisplay.h>
int parse_apic(U8* in) {
    if(validate_table(in)) {
        printk("Error:broken xsdt.\n");
        return -1;
    }
    
    return 0;
}