//解析ACPI的函数。
#include <acpi_parser.h>
#include <printk.h>
#include <xsdt.h>
#include <kstring.h>
#include <fadt.h>
#include <io.h>
#include <power_control.h>
//总共需要识别的表。
char* acpi_table_names[]={"FACP","APIC","SSDT","BGRT","DSDT"};
U8*   acpi_table_entries[sizeof(acpi_table_names)/8];
U8 table_count = sizeof(acpi_table_names)/sizeof(char*);
//解析RSDT的地址。
void* get_xsdt_addr(RSDPDescriptor20* rsdp) {
    return (void*)((U64)rsdp->XsdtAddress|KERNEL_ADDR_OFFSET);
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
    debug(" ACPI XSDP Version:%d\n",rsdp->firstPart.Revision); 
    //获取XSDT地址。
    void* xsdt = get_xsdt_addr(rsdp);
    //解析XSDT。
    if(parse_xsdt(xsdt)) {
        printk(" Invaild ACPI table.System halted.\n");
        halt();
    }
    
}
void acpi_interrupt_handler(void)
{
    debug(" Fired ACPI interrupt.\n");
    FADT* fadt = (FADT*)(acpi_table_entries[0]);
    U16 pm1_enable_port = (U16)fadt->X_PM1aEventBlock.Address;
    U16 action = inw((U16)(pm1_enable_port));
    outw(pm1_enable_port,1<<8);
    if((action&(1<<8))) {
        debug(" Now doing power off.\n");
        outw(pm1_enable_port,1<<8);
        poweroff();
    }
    if(pm1_enable_port=(U16)fadt->X_PM1bEventBlock.Address,pm1_enable_port) {
        action = inw((U16)(pm1_enable_port));
        if((action&(1<<8))) {
            debug(" Now doing power off.\n");
            outw(pm1_enable_port,1<<8);
            poweroff();
        }
    }
}