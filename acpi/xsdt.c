#include <xsdt.h>
#include <kerneldisplay.h>
#include <kstring.h>
void parse_xsdt(void* in) {
    char oemid[7];//For print ACPI Table OEM ID.
    char tableid[9];//For print ACPI Table ID.
    char sig[5]; //For print ACPI Signnature ID.
    //识别头。
    printk("The XDST header is at %x.\n",in);
    XSDT* xsdt = (XSDT*)in;
    strcopy(oemid,xsdt->header.OEMID,6);
    strcopy(sig,xsdt->header.Signature,4);
    printk("Table Signature:%s\n",sig);
    printk("Table OEM ID:%s\n",oemid);
    strcopy(tableid,xsdt->header.OEMTableID,8);
    printk("Table ID:%s\n",tableid);
    printk("Table length:%d\n",xsdt->header.Length);
}