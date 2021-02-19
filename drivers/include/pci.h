#ifndef PCI
#define PCI
#include <types.h>
#include <printk.h>
#include <io.h>
void init_pci();
#define pci_get_vendor(bus,slot,func) (pci_config_read_word(bus,slot,func,0x00))
#define pci_get_device(bus,slot,func) (pci_config_read_word(bus,slot,func,0x02))
#define pci_get_type(bus,slot,func)   (pci_config_read_word(bus,slot,func,0x0e)&0xFF)
const char* pci_get_class(U8 bus,U8 slot,U8 func);
#endif
