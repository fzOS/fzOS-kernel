#include <drivers/sata_ahci.h>
#include <common/popcount.h>
#include <interrupt/irq.h>
#include <memory/memory.h>
#include <drivers/devicetree.h>
#include <common/kstring.h>
#include <filesystem/filesystem.h>
#include <lai/helpers/pci.h>
#include <drivers/gpt.h>
#define AHCI_DEV_BUSY (1 << 7)
#define AHCI_DEV_DRQ (1 << 3)
#define MAX_BYTES_PER_PRDT (1 << 22)
#define ATAPI_COMMAND_LENGTH 12
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_IDENTIFY 0xEC
void ata_port_rebase(AHCIController device, HBA_PORT *port, int portno);
int ahci_readblock(block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount);
int ahci_writeblock(block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount);
static char* base_device_tree_template = "/Devices/";
static char* ata_controller_tree_template = "ATAController%d";
static char* ata_port_tree_template = "ATADevice%d";
static U8 SATA_device_count = 0;
int sata_identify(HBA_PORT* port, void *buffer);
char* AHCIDeviceTypeName[] =
{
    "NULL",
    "Serial ATA",
    "Serial Bridge",
    "Port Multiplier",
    "Serial ATAPI"
};
static inline void ata_parse_identify(AHCIDevice* device)
{
    U8* identify  = (U8*)&(device->identify);
    //现在已经没有硬盘不支持LBA48了……
    device->lba48_enabled = (identify[167]&0b100)?1:0;
    U64 lba_size = 0;
    lba_size |= (U64)identify[207] << 56;
    lba_size |= (U64)identify[206] << 48;
    lba_size |= (U64)identify[205] << 40;
    lba_size |= (U64)identify[204] << 32;
    lba_size |= (U64)identify[203] << 24;
    lba_size |= (U64)identify[202] << 16;
    lba_size |= (U64)identify[201] << 8;
    lba_size |= (U64)identify[200];
    device->sector_count = lba_size;
    //FIXME:修正sector大小。
    device->sector_size = 512;
    device->dev.block_size = 512;
}
void ata_interrupt_handler(int no)
{
    //首先，获取到AHCI的控制器。
    char buf[200];
    for(int i=0;i<SATA_device_count;i++) {
        sprintk(buf,"/Devices/ATAController%d/",i);
        AHCIControllerTreeNode* node = (AHCIControllerTreeNode*)device_tree_resolve_by_path(buf,DT_RETURN_IF_NONEXIST);
        if(node != nullptr) {
            if(node->controller.base.irq!=no) {
                continue;
            }
            else {
                U32 interrupts = node->controller.ahci_bar->is;
                //遍历。
                for(int j=0;j<node->controller.port_count;j++) {
                    if(interrupts & (1<<j)) {
                        HBA_PORT* port = &(node->controller.ahci_bar->ports[j]);
                        if(port->is & 0x01) {
                            //received.
                            port->is = 0;
                            return;
                        }
                    }
                }
            }
        }
    }
    printk(" SATA: Cannot determine which Controller!\n");
}
void sata_ahci_register(U8 bus,U8 slot,U8 func)
{
    //我们暂时只支持Intel的AHCI设备……
    //虽然AMD Yes但是没时间调试了23333
    AHCIController device;
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
    //重置适配器。
    device.ahci_bar->ghc |= (1 << 0);                             // Reset HBA
    while (device.ahci_bar->ghc & 0x01) __asm__ volatile("nop");  // Wait for reset
    device.ahci_bar->ghc |= (1 << 31);  // Enable ACHI mode
    device.ahci_bar->ghc |= (1 << 1);   // Set interrupt enable
    printk(" SATA:Got AHCI compatible device at PCI bus %d,slot %d,func %d.\n",bus,slot,func);
    //放入设备树中。
    char buf[200];
    //扫描端口。
    U32 port_count = popcount(device.ahci_bar->pi);
    device.port_count = port_count;
    device_tree_node* base_node = device_tree_resolve_by_path(base_device_tree_template,DT_CREATE_IF_NONEXIST);
    AHCIControllerTreeNode* controller_node = allocate_page(1);
    memset(controller_node,0,sizeof(AHCIControllerTreeNode));
    sprintk(buf,ata_controller_tree_template,SATA_device_count++);
    controller_node->controller = device;
    strcopy(controller_node->header.name,buf,DT_NAME_LENGTH_MAX);
    controller_node->header.type = DT_BLOCK_DEVICE;
    device_tree_add_from_parent((device_tree_node*)controller_node,(device_tree_node*)base_node);
    HBA_PORT* port;
    //尝试获取中断信息。
    union {
        U16 raw;
        U8 split[2];
    } interrupt_info = {
        .raw=pci_config_read_word(bus,slot,func,0x3C)
    };
    acpi_resource_t resource;
    if(lai_pci_route_pin(&resource,0,bus,slot,func,interrupt_info.split[1])!=LAI_ERROR_NONE) {
        printk(" LAI:Cannot find interrupt for ATA controller!\n");
    }
    irq_register(resource.base, 0xDD,0,0,ata_interrupt_handler);
    controller_node->controller.base.irq = resource.base;
    //为AHCI设备分配buffer空间。
    //Command List+FIS+Command Table
    U64 buffer_size_needed = (1024+256+8192)*port_count;
    device.command_base = (U64)(allocate_page(((buffer_size_needed-1)/PAGE_SIZE)+1))&0xFFFFFFFF;
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
        AHCIDeviceTreeNode* port_node = allocate_page((sizeof(AHCIDeviceTreeNode)/PAGE_SIZE)+1);
        memset(port_node,0,sizeof(AHCIDeviceTreeNode));
        port_node->controller = &(controller_node->controller);
        port_node->device.port = port;
        port_node->device.port_no = i;
        strcopy(port_node->header.name,buf,DT_NAME_LENGTH_MAX);
        port_node->header.type = DT_BLOCK_DEVICE;
        port_node->device.dev.readblock = ahci_readblock;
        port_node->device.dev.writeblock = ahci_writeblock;
        device_tree_add_from_parent((device_tree_node*)port_node,(device_tree_node*)controller_node);
        ata_port_rebase(device,port,i);
        ATA_IDENTIFY_DATA* identify_buffer = &(port_node->device.identify);
        if(type==AHCI_DEV_SATA) { //FIXME:IDENTIFY on SATAPI!.
            sata_identify(&(device.ahci_bar->ports[i]),identify_buffer);
            char name_buf[41];
            memcpy(name_buf,identify_buffer->ModelName,40);
            int k=39;
            while(name_buf[k]==' ') {
                k--;
            }
            name_buf[k-1]='\0';
            char c;
            for(;k>0;k-=2) {
                c = name_buf[k];
                name_buf[k] = name_buf[k-1];
                name_buf[k-1] = c;
            }
            ata_parse_identify(&port_node->device);
            printk(" AHCI %d-%d:%s device %s, %d bytes.\n",SATA_device_count-1,i, AHCIDeviceTypeName[type],name_buf,port_node->device.sector_count*port_node->device.sector_size);
            //测试。
            U64 ret = gpt_partition_init(&port_node->device.dev,&port_node->header);
            if(ret&FzOS_ROOT_PARTITION_FOUND) {
                sprintk(root_device_path,"/Devices/ATAController%d/ATADevice%d/Partition%d",SATA_device_count-1,i,ret&0x3FFFFFFF);
            }
        }
        else {
            printk(" AHCI %d-%d:%s device.\n",SATA_device_count-1,i,AHCIDeviceTypeName[type]);
        }
    }
}

static int find_command_slot(HBA_PORT *port)
{
    // If bit isn't set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    for (int i = 0; i < 32; i++) {
        if ((slots & (1 << i)) == 0) {
            return i;
        }
    }
    return FzOS_ERROR;
}
int ahci_begin_command(HBA_PORT *port)
{
    int slot = find_command_slot(port);
    return slot;
}

// Attempt to issue command, true when completed, false if error
int ahci_issue_command(HBA_PORT* port, int slot)
{

    // Wait until the port is free
    int spin = 0;
    while ((port->tfd & (AHCI_DEV_BUSY | AHCI_DEV_DRQ)) && spin < 1000000) {
        spin++;
    }
    if (spin == 1000000) {
        return FzOS_DEVICE_NOT_READY;
    }

    port->serr = 0xFFFFFFFF;

    port->ci |= 1 << slot;
    while (port->ci & (1 << slot)) {
        //FIXME:Use Semaphore.
    }
    return FzOS_SUCEESS;
}
// Start command engine
void ata_start_cmd(HBA_PORT *port)
{
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
    port->ie = 0xFFFFFFFF;
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
void ata_port_rebase(AHCIController device, HBA_PORT *port, int portno)
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
    port->fb = device.command_base + (device.port_count<<10) + (portno<<8);
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
        cmdheader[i].ctba = device.command_base + (device.port_count*1280) + (portno<<13) + (i<<8);
        cmdheader[i].ctbau = 0;
        memset((void*)(cmdheader[i].ctba|KERNEL_ADDR_OFFSET), 0, 256);
    }
    ata_start_cmd(port); // Start command engine
}
FIS_REG_H2D *ahci_initialize_command_fis(HBA_PORT* port,
                                         int slot,
                                         AHCIOperationType write,
                                         AHCIPrefetchType prefetchable,
                                         U64 byte_size,
                                         U8  *dma_buffer,
                                         U8  atapi,
                                         U8  *atapi_command)
{

    // Get the command header associated with our free slot
    // TODO: Refactor this into a #define or something
    U64 command_header_address = port->clb;
    command_header_address |= ((U64)port->clbu)<< 32;
    command_header_address += slot * sizeof(HBA_CMD_HEADER);
    HBA_CMD_HEADER *command_header = (HBA_CMD_HEADER*)command_header_address;
    memset(command_header, 0, sizeof(HBA_CMD_HEADER));
    command_header->cfl = sizeof(FIS_REG_H2D) / sizeof(U32);  // Command FIS size in dwords
    command_header->w = write;
    command_header->p = prefetchable;
    command_header->prdtl = ((byte_size - 1) / MAX_BYTES_PER_PRDT) + 1;

    // Get command table from command header
    U64 command_table_address =
        command_header->ctba;
    command_table_address |=
        ((U64)command_header->ctbau) << 32;

    HBA_CMD_TBL *command_table = (HBA_CMD_TBL*)command_table_address;
    memset(command_table, 0,sizeof(HBA_CMD_TBL) +(command_header->prdtl * sizeof(HBA_PRDT_ENTRY)));

    if (atapi && atapi_command) {
        command_header->a = 1;
        memcpy(command_table->acmd, atapi_command, ATAPI_COMMAND_LENGTH);
    }

    // Get PRDT from command table
    HBA_PRDT_ENTRY *prdt = command_table->prdt_entry;
    val_splitter splitter;
    // Fill PRDT. 4MB (8192 sectors) per PRDT entrt
    for (int i = 0; i < command_header->prdtl; ++i) {
        splitter.raw = (U64)dma_buffer;
        prdt[i].dba = splitter.dword[0];
        prdt[i].dbau = splitter.dword[1];
        // data_size is 0-indexed
        prdt[i].dbc =
            byte_size > MAX_BYTES_PER_PRDT ? MAX_BYTES_PER_PRDT - 1 : byte_size - 1;
        prdt[i].i = 1;
        dma_buffer += prdt[i].dbc + 1;
        byte_size -= prdt[i].dbc + 1;
    }
    // Setup command
    FIS_REG_H2D *command_fis =(FIS_REG_H2D *)(&command_table->cfis);
    memset(command_fis, 0, sizeof(FIS_REG_H2D));
    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->c = 1;
    return command_fis;
}

int sata_identify(HBA_PORT* port, void *buffer) {
    buffer = (void*)((U64)buffer&(~KERNEL_ADDR_OFFSET));
    int slot = ahci_begin_command(port);
    if (slot == -1) {
        return FzOS_ERROR;
    }
    // Setup command
    FIS_REG_H2D *command_fis = ahci_initialize_command_fis(
        port, slot, AHCI_COMMAND_H2D, AHCI_COMMAND_PREFETCHABLE, 512, buffer, 0, NULL);
    command_fis->command = ATA_CMD_IDENTIFY;
    command_fis->device = 0;
    return ahci_issue_command(port, slot);
}


void ahci_set_command_fis_lba(FIS_REG_H2D *command_fis, U64 address,
                              U64 block_count) {
    val_splitter splitter;
    splitter.raw = address;
    command_fis->lba0 = splitter.byte[0];
    command_fis->lba1 = splitter.byte[1];
    command_fis->lba2 = splitter.byte[2];
    command_fis->device = 1 << 6;  // LBA mode;

    command_fis->lba3 = splitter.byte[3];
    command_fis->lba4 = splitter.byte[4];
    command_fis->lba5 = splitter.byte[5];

    splitter.raw = block_count;
    command_fis->countl = splitter.byte[0];
    command_fis->counth = splitter.byte[1];
}
int sata_rw_command(ATAOperationType type, AHCIDevice *device,
                                     U64 address, U64 block_count,
                                     void *buffer, U64 buffer_size) {
    buffer = (void*)((U64)buffer&(~KERNEL_ADDR_OFFSET));
    if (buffer_size < block_count * device->sector_size) {
        return FzOS_BUFFER_TOO_SMALL;
    }

    const int slot = ahci_begin_command(device->port);
    if (slot == -1) {
        return FzOS_DEVICE_NOT_READY;
    }

    // Setup command
    U64 requested_bytes = block_count * device->sector_size;
    FIS_REG_H2D *command_fis = ahci_initialize_command_fis(
        device->port, slot,(type==ATA_OPERATION_WRITE?AHCI_COMMAND_D2H:AHCI_COMMAND_H2D),AHCI_COMMAND_PREFETCHABLE, requested_bytes, buffer, 0, NULL);

    if (device->lba48_enabled) {
        command_fis->command = (type==ATA_OPERATION_WRITE) ? ATA_CMD_WRITE_DMA_EX : ATA_CMD_READ_DMA_EX;
    } else {
        command_fis->command = (type==ATA_OPERATION_WRITE) ? ATA_CMD_WRITE_DMA : ATA_CMD_READ_DMA;
    }

    ahci_set_command_fis_lba(command_fis, address, block_count);
    return ahci_issue_command(device->port, slot) ? FzOS_SUCEESS
                                            : FzOS_DEVICE_NOT_READY;
}
int ahci_readblock(block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount)
{
    AHCIDevice* device = (AHCIDevice*) dev;
    return sata_rw_command(ATA_OPERATION_READ, device, offset, blockcount, buffer,
                         buffer_size);
}
int ahci_writeblock(block_dev* dev,U64 offset,void* buffer,U64 buffer_size,U64 blockcount)
{
    AHCIDevice* device = (AHCIDevice*) dev;
    return sata_rw_command(ATA_OPERATION_WRITE, device, offset, blockcount, buffer,
                         buffer_size);
}
