#include <drivers/sata_ahci.h>
#define sata_debug(x...) debug(" sata ahci:" x)
void sata_ahci_register(U8 bus,U8 slot,U8 func)
{
    //我们暂时只支持Intel的AHCI设备……
    //虽然AMD Yes但是没时间调试了23333
    AHCIDevice device;
    device.base.bus = bus;
    device.base.slot = slot;
    device.base.func = func;
    U16 vendor = pci_get_vendor(bus,slot,func);
    if(vendor!=0x8086) {
        printk(" Error:unsupported SATA Vendor.\n");
        return;
    }
    device.ahci_bar = (void*)((U64)pci_get_bar_address(bus,slot,func,5)|KERNEL_ADDR_OFFSET);
    if(!(device.ahci_bar->ghc&0x80000000)) {
        printk(" Error:SATA Controller not in AHCI mode.\n");
        return;
    }
    sata_debug("Got AHCI Compatible Device at PCI bus %d,slot %d,func %d.\n",bus,slot,func);
    sata_debug("This SATA Controller has %d ports.\n",device.ahci_bar->pi);
}
