#include <drivers/pci.h>
#include <drivers/sata_ahci.h>
#include <drivers/hda.h>
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
    "Reserved",
};
const char* pci_subclass_0_name[] = {
    "Non-VGA-Compatible devices",
    "VGA-Compatible Device",
};
const char* pci_subclass_1_name[] = {
    "SCSI Bus Controller",
    "IDE Controller",
    "Floppy Disk Controller",
    "IPI Bus Controller",
    "RAID Controller",
    "ATA Controller",
    "Serial ATA",
    "Serial Attached SCSI",
    "Non-Volatile Memory Controller",
};
const char* pci_subclass_2_name[] = {
    "Ethernet Controller",
    "Token Ring Controller",
    "FDDI Controller",
    "ATM Controller",
    "ISDN Controller",
    "WorldFip Controller",
    "PICMG 2.14 Multi Computing",
    "Fabric Controller",
};
const char* pci_subclass_3_name[] = {
    "VGA Compatible Controller",
    "XGA Controller",
    "3D Controller (Not VGA-Compatible)",
};
const char* pci_subclass_4_name[] = {
    "Multimedia Video Controller",
    "Multimedia Audio Controller",
    "Computer Telephony Device",
    "Audio Device",
};
const char* pci_subclass_5_name[] = {
    "RAM Controller",
    "Flash Controller",
};
const char* pci_subclass_6_name[] = {
    "Host Bridge",
    "ISA Bridge",
    "EISA Bridge",
    "MCA Bridge",
    "PCI-to-PCI Bridge",
    "PCMCIA Bridge",
    "NuBus Bridge",
    "CardBus Bridge",
    "RACEway Bridge",
    "PCI-to-PCI Bridge",
    "InfiniBand-to-PCI Host Bridge",
};
const char* pci_subclass_7_name[] = {
    "Serial Controller",
    "Parallel Controller",
    "Multiport Serial Controller",
    "Modem",
    "IEEE 488.1/2 (GPIB) Controller",
    "Smart Card",
};
const char* pci_subclass_8_name[] = {
    "PIC",
    "DMA Controller",
    "Timer",
    "RTC Controller",
    "PCI Hot-Plug Controller",
    "SD Host controller",
    "IOMMU",
};
const char* pci_subclass_9_name[] = {
    "Keyboard Controller",
    "Digitizer Pen",
    "Mouse Controller",
    "Scanner Controller",
    "Gameport Controller",
};
const char* pci_subclass_a_name[] = {
    "Generic",
};
const char* pci_subclass_b_name[] = {
    "386",
    "486",
    "Pentium",
    "Pentium Pro",
};
const char* pci_subclass_c_name[] = {
    "FireWire (IEEE 1394) Controller",
    "ACCESS Bus",
    "SSA",
    "USB Controller",
    "Fibre Channel",
    "SMBus",
    "InfiniBand",
    "IPMI Interface",
    "SERCOS Interface (IEC 61491)",
    "CANbus",
};
const char* pci_subclass_d_name[] = {
    "iRDA Compatible Controller",
    "Consumer IR Controller",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "RF Controller",
    "Bluetooth Controller",
    "Broadband Controller",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ethernet Controller (802.1a)",
    "Ethernet Controller (802.1b)",
};
const char* pci_subclass_e_name[] = {
    "I20",
};
const char* pci_subclass_f_name[] = {
    "Satellite TV Controller",
    "Satellite Audio Controller",
    "Satellite Voice Controller",
    "Satellite Data Controller",
};
const char* pci_subclass_10_name[] = {
    "Network and Computing Encrpytion/Decryption",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Entertainment Encryption/Decryption",
};
const char* pci_subclass_11_name[] = {
    "DPIO Modules",
    "Performance Counters",
};
static const int pci_class_name_size = sizeof(pci_class_name)/sizeof(unsigned char*);
static const char** pci_subclass_name[] = {
    pci_subclass_0_name,
    pci_subclass_1_name, 
    pci_subclass_2_name, 
    pci_subclass_3_name, 
    pci_subclass_4_name, 
    pci_subclass_5_name, 
    pci_subclass_6_name, 
    pci_subclass_7_name, 
    pci_subclass_8_name, 
    pci_subclass_9_name, 
    pci_subclass_a_name, 
    pci_subclass_b_name, 
    pci_subclass_c_name, 
    pci_subclass_d_name, 
    pci_subclass_e_name, 
    pci_subclass_f_name, 
    pci_subclass_10_name, 
    pci_subclass_11_name, 
};
static const int pci_subclass_name_size[] = {
    sizeof(pci_subclass_0_name)/sizeof(const char*),
    sizeof(pci_subclass_1_name)/sizeof(const char*), 
    sizeof(pci_subclass_2_name)/sizeof(const char*), 
    sizeof(pci_subclass_3_name)/sizeof(const char*), 
    sizeof(pci_subclass_4_name)/sizeof(const char*), 
    sizeof(pci_subclass_5_name)/sizeof(const char*), 
    sizeof(pci_subclass_6_name)/sizeof(const char*), 
    sizeof(pci_subclass_7_name)/sizeof(const char*), 
    sizeof(pci_subclass_8_name)/sizeof(const char*), 
    sizeof(pci_subclass_9_name)/sizeof(const char*), 
    sizeof(pci_subclass_a_name)/sizeof(const char*), 
    sizeof(pci_subclass_b_name)/sizeof(const char*), 
    sizeof(pci_subclass_c_name)/sizeof(const char*), 
    sizeof(pci_subclass_d_name)/sizeof(const char*), 
    sizeof(pci_subclass_e_name)/sizeof(const char*), 
    sizeof(pci_subclass_f_name)/sizeof(const char*), 
    sizeof(pci_subclass_10_name)/sizeof(const char*), 
    sizeof(pci_subclass_11_name)/sizeof(const char*),
}; 
U16 pci_config_read_word (U8 bus, U8 device, U8 func, U8 offset)
{
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    U16 tmp = 0;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xff) |  0x80<<24);
    outl(0xCF8, address);
    U32 result = inl(0xCFC);
    tmp = (U16)((result >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}
U32 pci_read_dword(U8 bus, U8 device, U8 func, U8 offset)
{
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    outl(0xCF8, address);
    U32 result = inl(0xCFC);
    return result;
}
U8 pci_read_byte(U8 bus, U8 device, U8 func, U8 offset)
{
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    outl(0xCF8, address);
    U32 result = inl(0xCFC);
    return (U8)((result >> ((offset %4) * 8)) & 0xff);
}
void pci_write_dword(U8 bus, U8 device, U8 func, U8 offset,U32 val)
{
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    outl(0xCF8, address);
    outl(0xCFC, val);
}
void pci_write_word(U8 bus, U8 device, U8 func, U8 offset,U16 val)
{
    U32 orig_val = pci_config_read_word(bus,device,func,offset);
    U32 mask = (0xffff << ((offset & 2) * 8));
    orig_val &= (~mask);
    orig_val |= (val << ((offset & 2) * 8));
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    outl(0xCF8, address);
    outw(0xCFC, orig_val);
}
void pci_write_byte(U8 bus, U8 device, U8 func, U8 offset,U8 val)
{
    U32 orig_val = pci_read_byte(bus,device,func,offset);
    U32 mask = (0xff << ((offset %4) * 8));
    orig_val &= (~mask);
    orig_val |= (val << ((offset %4) * 8));
    U32 address;
    U32 lbus  = (U32)bus;
    U32 ldevice = (U32)device;
    U32 lfunc = (U32)func;
    address = (U32)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) |  0x80<<24);
    outl(0xCF8, address);
    outl(0xCFC, val);
}
void pci_check_device(U8 bus,U8 slot,U8 func)
{
    U8 class = pci_get_class(bus,slot,func);
    U8 subclass = pci_get_subclass(bus,slot,func);
    printk(" %d:%d:%d %s %w %w\n",bus,slot,func,
                                  pci_get_class_name(class,subclass),
                                  pci_get_vendor(bus,slot,func),
                                  pci_get_device(bus,slot,func));
    //注册AHCI设备。
    if(class==0x01 && subclass ==0x06) {
        sata_ahci_register(bus,slot,func);
    }
    //注册HDA设备。
    if(class==0x04 && subclass ==0x03) {
        hda_register(bus,slot,func);
    }
}
void pci_check_bus(U8 bus) 
{
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
     U8 bus;
     for(bus = 0; bus < 255; bus++) {
        pci_check_bus(bus);
     }
}
void init_pci()
{
    printk(" [PCI Configuration Begin]\n");
    pci_check_all_buses();
    printk(" [PCI Configuration End]\n");
}
const char* pci_get_class_name(U8 class, U8 subclass)
{
    if(class >= pci_class_name_size) {
        return pci_class_name[pci_class_name_size-1];
    }
    if(subclass>=pci_subclass_name_size[class]||*(pci_subclass_name[class][subclass])=='\0') {
        return pci_class_name[class];        
    }
    return pci_subclass_name[class][subclass];
}
