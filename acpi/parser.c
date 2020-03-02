//解析ACPI的函数。
#include <acpi_parser.h>
#include <kerneldisplay.h>
#include <xsdt.h>
//解析RSDT的地址。
void* get_xsdt_addr(RSDPDescriptor20* rsdp) {
    return (void*)(rsdp->XsdtAddress);
}
//输出RSDT中ACPI表的基本信息。
void parse_acpi(U8* in) {
    char oemid[7];//For print ACPI Table OEM ID.
    char tableid[9];//For print ACPI Table ID.
    char sig[5]; //For print ACPI Signnature ID.
    //由于现在真的很难找到ACPI 1.0的设备，我们假设至少有ACPI 2.0。
    RSDPDescriptor20* rsdp = (RSDPDescriptor20*) in;
    for(int j=0;j<6;j++)
        oemid[j] = rsdp->firstPart.OEMID[j];
    oemid[6]=0;
    printk("%s ACPI RSDP Version:%d\n",oemid,rsdp->firstPart.Revision); 
    //获取XSDT地址。
    void* xsdt = get_xsdt_addr(rsdp);
    //解析XSDT。
    parse_xsdt(xsdt);
    
}