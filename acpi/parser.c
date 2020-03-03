//解析ACPI的函数。
#include <acpi_parser.h>
#include <kerneldisplay.h>
#include <xsdt.h>
#include <kstring.h>
//解析RSDT的地址。
void* get_xsdt_addr(RSDPDescriptor20* rsdp) {
    return (void*)((U64)rsdp->XsdtAddress);
}
//校验表。
int validate_table(U8* in) {
    int result=0;
    int count=((ACPISDTHeader*)in)->Length;
    for(int i=0;i<count;i++) {
        result += in[i];
    }
    return result&0xFF;
}
//ACPI表读取的入口。
void parse_acpi(U8* in) {
    //由于现在真的很难找到ACPI 1.0的设备，我们假设至少有ACPI 2.0。
    RSDPDescriptor20* rsdp = (RSDPDescriptor20*) in;
    printk("ACPI XSDP Version:%d\n",rsdp->firstPart.Revision); 
    //获取XSDT地址。
    void* xsdt = get_xsdt_addr(rsdp);
    //解析XSDT。
    parse_xsdt(xsdt);
    
}