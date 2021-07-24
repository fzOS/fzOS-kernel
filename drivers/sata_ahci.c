#include <drivers/sata_ahci.h>
#include <common/popcount.h>
//#include <lai/helpers/pci.h>
#include <interrupt/irq.h>
#include <memory/memory.h>
#include <drivers/devicetree.h>
#include <common/kstring.h>
#define sata_debug(x...) debug(" sata ahci:" x)
void ata_port_rebase(AHCIDevice device, HBA_PORT *port, int portno);
static char* ata_controller_tree_template = "/Devices/ATAController%d/";
static char* ata_port_tree_template = "ATADevice%d";
static U8 SATA_device_count = 0;
char* AHCIDeviceTypeName[] =
{
    "NULL",
    "Serial ATA",
    "Serial Bridge",
    "Port Multiplier",
    "Serial ATAPI"
};
void ata_interrupt_handler(void)
{
    debug(" Got info from ATA!\n");
}
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
    printk(" SATA:Got AHCI compatible device at PCI bus %d,slot %d,func %d.\n",bus,slot,func);
    //放入设备树中。
    char buf[200];
    sprintk(buf,ata_controller_tree_template,SATA_device_count++);
    device_tree_node* controller_node = device_tree_resolve_by_path(buf,DT_CREATE_IF_NONEXIST);
    //扫描端口。
    U32 port_count = popcount(device.ahci_bar->pi);
    sata_debug("This SATA Controller has %d ports.\n", port_count);
    HBA_PORT* port;
    for(U32 i=0;i<port_count;i++) {
        port = &(device.ahci_bar->ports[i]);
        U32 ssts = port->ssts;
        U8 ipm = (ssts >> 8) & 0x0F;
        U8 det = ssts & 0x0F;
        U8 type=AHCI_DEV_NULL;
        if (det!=HBA_PORT_DET_PRESENT || ipm!=HBA_PORT_IPM_ACTIVE) {// Check drive status
            continue;
        }

        switch (port->sig) {
            case SATA_SIG_ATAPI: {
                type = AHCI_DEV_SATAPI;
                break;
            }
            case SATA_SIG_SEMB: {
                type = AHCI_DEV_SEMB;
                break;
            }
            case SATA_SIG_PM: {
                type = AHCI_DEV_PM;
                break;
            }
            default: {
                type = AHCI_DEV_SATA;
                break;
            }
        }
        sprintk(buf,ata_port_tree_template,i);
        AHCIDeviceTreeNode* port = allocate_page(1);
        memset(port,0,sizeof(AHCIDeviceTreeNode));
        port->controller = device;
        strcopy(port->header.base.name,buf,DT_NAME_LENGTH_MAX);
        device_tree_add_from_parent((device_tree_node*)port,(device_tree_node*)controller_node);
        sata_debug(" Got %s device on port %d.\n",AHCIDeviceTypeName[type],i);
    }
    //尝试获取中断信息。
    union {
        U16 raw;
        U8 split[2];
    } interrupt_info = {
        .raw=pci_config_read_word(bus,slot,func,0x3C)
    };
    //就当他给的对吧……
    irq_register(interrupt_info.split[0], 0xDD,0,0,ata_interrupt_handler);
    //为AHCI设备分配buffer空间。
    U64 buffer_size_needed = 1024*port_count;
    device.command_base = (U64)(allocate_page(((buffer_size_needed-1)/PAGE_SIZE)+1))&0xFFFFFFFF;
//     acpi_resource_t resource;
//     if(lai_pci_route_pin(&resource,0,bus,slot,func,interrupt_info.split[1])!=LAI_ERROR_NONE) {
//         printk(" Error!\n");
//     }
//     else {
//         printk(" %x\n",resource.base);
//     }
    for(int i=0;i<port_count;i++) {
        ata_port_rebase(device,&(device.ahci_bar->ports[i]),i);
    }
    print_device_tree();
}

// Start command engine
void ata_start_cmd(HBA_PORT *port)
{
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}
// Stop command engine
void ata_stop_cmd(HBA_PORT *port)
{
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;
    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }
}
void ata_port_rebase(AHCIDevice device, HBA_PORT *port, int portno)
{
    ata_stop_cmd(port); // Stop command engine
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = device.command_base + (portno<<10);
    port->clbu = 0;
    memset((void*)((port->clb)|KERNEL_ADDR_OFFSET), 0, 1024);
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = device.command_base + (32<<10) + (portno<<8);
    port->fbu = 0;
    memset((void*)((port->fb)|KERNEL_ADDR_OFFSET), 0, 256);
    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)((port->clb)|KERNEL_ADDR_OFFSET);
    for (int i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = device.command_base + (40<<10) + (portno<<13) + (i<<8);
        cmdheader[i].ctbau = 0;
        memset((void*)(cmdheader[i].ctba|KERNEL_ADDR_OFFSET), 0, 256);
    }
    ata_start_cmd(port); // Start command engine
}
