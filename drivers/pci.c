#include <pci.h>

const char* pci_class_name[] = {
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station",
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Reserved"
};
static const int pci_class_name_size = sizeof(pci_class_name)/sizeof(unsigned char*);
U16 pci_config_read_word (U8 bus, U8 device, U8 func, U8 offset)
{
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    U16 tmp = 0;
    /* create configuration address as per Figure 1 */
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    U32 result = inl(0xCFC);
    tmp = (U16)((result >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}
void pci_check_device(U8 bus,U8 slot,U8 func)
{
     printk("%d:%d:%d %s %w %w\n",bus,slot,func,
                                  pci_get_class(bus,slot,func),
                                  pci_get_vendor(bus,slot,func),
                                  pci_get_device(bus,slot,func));
}
void pci_check_bus(U8 bus) {
    for(U8 slot = 0; slot < 32; slot++) {
        if(pci_get_vendor(bus,slot,0)!=0xFFFF) {
            pci_check_device(bus,slot,0);
            if(pci_get_type(bus,slot,0)&0x80) {
                //Multi-func device.
                //F**k Intel!
                //It makes me lose my AHCI Controller.
                for(U8 func=1;func<8;func++) {
                    if(pci_get_vendor(bus,slot,func)!=0xFFFF) {
                        pci_check_device(bus,slot,func);
                    }
                }
            }
        }
    }
 }
void pci_check_all_buses(void) {
    //U8 bus;
/* 
     headerType = getHeaderType(0, 0, 0);
     if( (headerType & 0x80) == 0) {
         ///Single PCI host controller 
         checkBus(0);
     } else {
         // Multiple PCI host controllers
         for(function = 0; function < 8; function++) {
             if(getVendorID(0, 0, function) != 0xFFFF) break;
             bus = function;
             checkBus(bus);
         }
     }

    U8 header_type = get_type(0,0,0);
    printk("PCI Root Host Controller: %w:%w\n",get_vendor(0,0,0),get_device(0,0,0));
    if(header_type&0x80) {
        
    }
*/
     U8 bus;
     for(bus = 0; bus < 255; bus++) {
        pci_check_bus(bus);
     }
     printk("Done.\n");
}
void init_pci()
{
    printk("[PCI Configuration Begin]\n");
    pci_check_all_buses();
}

const char* pci_get_class(U8 bus,U8 slot,U8 func)
{
    U8 class = ((pci_config_read_word(bus,slot,func,0x0a)&0xFF00)>>8);
    if(class >= pci_class_name_size) {
        return pci_class_name[pci_class_name_size-1];
    }
    return pci_class_name[class];
}
