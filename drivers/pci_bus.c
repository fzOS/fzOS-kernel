#include <drivers/pci.h>
#include <drivers/sata_ahci.h>
#include <drivers/hda.h>
#include <drivers/vmsvga.h>
const char* PCI_CLASS_NAME[] = {
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
const char* PCI_SUBCLASS_0_NAME[] = {
    "Non-VGA-Compatible devices",
    "VGA-Compatible Device",
};
const char* PCI_SUBCLASS_1_NAME[] = {
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
const char* PCI_SUBCLASS_2_NAME[] = {
    "Ethernet Controller",
    "Token Ring Controller",
    "FDDI Controller",
    "ATM Controller",
    "ISDN Controller",
    "WorldFip Controller",
    "PICMG 2.14 Multi Computing",
    "Fabric Controller",
};
const char* PCI_SUBCLASS_3_NAME[] = {
    "VGA Compatible Controller",
    "XGA Controller",
    "3D Controller (Not VGA-Compatible)",
};
const char* PCI_SUBCLASS_4_NAME[] = {
    "Multimedia Video Controller",
    "Multimedia Audio Controller",
    "Computer Telephony Device",
    "Audio Device",
};
const char* PCI_SUBCLASS_5_NAME[] = {
    "RAM Controller",
    "Flash Controller",
};
const char* PCI_SUBCLASS_6_NAME[] = {
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
const char* PCI_SUBCLASS_7_NAME[] = {
    "Serial Controller",
    "Parallel Controller",
    "Multiport Serial Controller",
    "Modem",
    "IEEE 488.1/2 (GPIB) Controller",
    "Smart Card",
};
const char* PCI_SUBCLASS_8_NAME[] = {
    "PIC",
    "DMA Controller",
    "Timer",
    "RTC Controller",
    "PCI Hot-Plug Controller",
    "SD Host controller",
    "IOMMU",
};
const char* PCI_SUBCLASS_9_NAME[] = {
    "Keyboard Controller",
    "Digitizer Pen",
    "Mouse Controller",
    "Scanner Controller",
    "Gameport Controller",
};
const char* PCI_SUBCLASS_A_NAME[] = {
    "Generic",
};
const char* PCI_SUBCLASS_B_NAME[] = {
    "386",
    "486",
    "Pentium",
    "Pentium Pro",
};
const char* PCI_SUBCLASS_C_NAME[] = {
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
const char* PCI_SUBCLASS_D_NAME[] = {
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
const char* PCI_SUBCLASS_E_NAME[] = {
    "I20",
};
const char* PCI_SUBCLASS_F_NAME[] = {
    "Satellite TV Controller",
    "Satellite Audio Controller",
    "Satellite Voice Controller",
    "Satellite Data Controller",
};
const char* PCI_SUBCLASS_10_NAME[] = {
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
const char* PCI_SUBCLASS_11_NAME[] = {
    "DPIO Modules",
    "Performance Counters",
};
static const int PCI_CLASS_NAME_SIZE = sizeof(PCI_CLASS_NAME)/sizeof(unsigned char*);
static const char** PCI_SUBCLASS_NAME[] = {
    PCI_SUBCLASS_0_NAME,
    PCI_SUBCLASS_1_NAME,
    PCI_SUBCLASS_2_NAME,
    PCI_SUBCLASS_3_NAME,
    PCI_SUBCLASS_4_NAME,
    PCI_SUBCLASS_5_NAME,
    PCI_SUBCLASS_6_NAME,
    PCI_SUBCLASS_7_NAME,
    PCI_SUBCLASS_8_NAME,
    PCI_SUBCLASS_9_NAME,
    PCI_SUBCLASS_A_NAME,
    PCI_SUBCLASS_B_NAME,
    PCI_SUBCLASS_C_NAME,
    PCI_SUBCLASS_D_NAME,
    PCI_SUBCLASS_E_NAME,
    PCI_SUBCLASS_F_NAME,
    PCI_SUBCLASS_10_NAME,
    PCI_SUBCLASS_11_NAME,
};
static const int PCI_SUBCLASS_NAME_SIZE[] = {
    sizeof(PCI_SUBCLASS_0_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_1_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_2_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_3_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_4_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_5_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_6_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_7_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_8_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_9_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_A_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_B_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_C_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_D_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_E_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_F_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_10_NAME)/sizeof(const char*),
    sizeof(PCI_SUBCLASS_11_NAME)/sizeof(const char*),
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
    outl(0xCFC, orig_val);
}
void pci_check_device(U8 bus,U8 slot,U8 func)
{
    U8 class = pci_get_class(bus,slot,func);
    U8 subclass = pci_get_subclass(bus,slot,func);
    printk(" %d:%d:%d %s 0x%w 0x%w\n",bus,slot,func,
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
    //注册显示设备。
    if(class==0x03 && subclass ==0x00) {
        vmsvga_register(bus,slot,func);
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
    if(class >= PCI_CLASS_NAME_SIZE) {
        return PCI_CLASS_NAME[PCI_CLASS_NAME_SIZE-1];
    }
    if(subclass>=PCI_SUBCLASS_NAME_SIZE[class]||*(PCI_SUBCLASS_NAME[class][subclass])=='\0') {
        return PCI_CLASS_NAME[class];
    }
    return PCI_SUBCLASS_NAME[class][subclass];
}
