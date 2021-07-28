#ifndef PCI
#define PCI
#include <types.h>
#include <common/printk.h>
#include <common/io.h>
void init_pci();
#define pci_get_vendor(bus,slot,func)   (pci_config_read_word(bus,slot,func,0x00))
#define pci_get_device(bus,slot,func)   (pci_config_read_word(bus,slot,func,0x02))
#define pci_get_type(bus,slot,func)     (pci_config_read_word(bus,slot,func,0x0e)&0xFF)
#define pci_get_class(bus,slot,func)    ((pci_config_read_word(bus,slot,func,0x0a)&0xFF00)>>8)
#define pci_get_subclass(bus,slot,func) (pci_config_read_word(bus,slot,func,0x0a)&0xFF)
#define pci_get_bar_address(bus,slot,func,count)    (pci_read_dword(bus,slot,func,0x10+(count<<2)))
const char* pci_get_class_name(U8 class, U8 subclass);
U16 pci_config_read_word (U8 bus, U8 device, U8 func, U8 offset);
U32 pci_read_dword(U8 bus, U8 device, U8 func, U8 offset);
U8 pci_read_byte(U8 bus, U8 device, U8 func, U8 offset);
void pci_write_byte(U8 bus, U8 device, U8 func, U8 offset,U8 val);
void pci_write_word(U8 bus, U8 device, U8 func, U8 offset,U16 val);
void pci_write_dword(U8 bus, U8 device, U8 func, U8 offset,U32 val);
typedef struct
{
    U8 bus;
    U8 slot;
    U8 func;
    U8 irq;
} PCIDevice;
#endif
