#include <xsdt.h>
#include <kerneldisplay.h>
#include <kstring.h>
void parse_xsdt(void* in) {
    //检测表。
    if(validate_table((U8*)in)) {
        printk("Error:broken xsdt.\n");
        return;
    }
    char tableid[9];//For print ACPI Table ID.
    char sig[5]; //For print ACPI Signnature ID.
    //识别头。
    XSDT* xsdt = (XSDT*)in;
    int entry_count = xsdt->header.Length = sizeof(xsdt->header) / sizeof(U64);
    printk("There are %d entries in XSDT Table.\n",entry_count);
    U64* entries = (&xsdt->PointerToOtherSDT);
    for(int i=0;i<entry_count;i++) {
        ACPISDTHeader* current_header = (ACPISDTHeader*)entries[i];
        strcopy(tableid,current_header->OEMTableID,8);
        strcopy(sig,current_header->Signature,4);
        printk("Entry #%d:%x %s %s\n",i,entries[i],tableid,sig);
    }
}