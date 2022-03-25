#include <drivers/vmsvga.h>
#include <common/io.h>
#include <drivers/graphics.h>
static U64 g_io_port_base;
static U32* g_fifo_address;
void vmsvga_write_register(U32 index,U32 value)
{
   outl(g_io_port_base + SVGA_INDEX_PORT, index);
   outl(g_io_port_base + SVGA_VALUE_PORT, value);
}
U32 vmsvga_read_register(U32 index)
{
   outl(g_io_port_base + SVGA_INDEX_PORT, index);
   return inl(g_io_port_base + SVGA_VALUE_PORT);
}
void vmsvga_register(U8 bus,U8 slot,U8 func)
{
    U16 vendor = pci_get_vendor(bus,slot,func);
    U16 device = pci_get_device(bus,slot,func);
    if(vendor!=0x15AD||device!=0x0405) {
        return;
    }
    printk(" VMSVGA:Got VMSVGA II at PCI bus %d,slot %d,func %d.\n",bus,slot,func);
    g_io_port_base = (U64)pci_get_bar_address(bus,slot,func,0)-1;//Why do we need -1?Virtualbox bug?
    g_fifo_address = (U32*)(U64)pci_get_bar_address(bus,slot,func,2);
    U32 fifo_size = vmsvga_read_register(SVGA_REG_MEM_SIZE);
    vmsvga_write_register(SVGA_REG_ENABLE, TRUE);
    g_fifo_address[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(U32);
    g_fifo_address[SVGA_FIFO_MAX] = fifo_size;
    g_fifo_address[SVGA_FIFO_NEXT_CMD] = g_fifo_address[SVGA_FIFO_MIN];
    g_fifo_address[SVGA_FIFO_STOP] = g_fifo_address[SVGA_FIFO_MIN];
    vmsvga_write_register(SVGA_REG_CONFIG_DONE, TRUE);
    return;
}
