#ifndef SATA_AHCI
#define SATA_AHCI
#include <drivers/pci.h>
#include <drivers/blockdev.h>
#include <drivers/devicetree.h>
typedef enum
{
    FIS_TYPE_REG_H2D = 0x27, // Register FIS - host to device
    FIS_TYPE_REG_D2H = 0x34, // Register FIS - device to host
    FIS_TYPE_DMA_ACT = 0x39, // DMA activate FIS - device to host
    FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
    FIS_TYPE_DATA = 0x46, // Data FIS - bidirectional
    FIS_TYPE_BIST = 0x58, // BIST activate FIS - bidirectional
    FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
    FIS_TYPE_DEV_BITS = 0xA1, // Set device bits FIS - device to host
} FIS_TYPE;

#define SATA_SIG_ATA 0x00000101 // SATA drive
#define SATA_SIG_ATAPI 0xEB140101 // SATAPI drive
#define SATA_SIG_SEMB 0xC33C0101 // Enclosure management bridge
#define SATA_SIG_PM 0x96690101 // Port multiplier
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000
typedef enum
{
    AHCI_DEV_NULL,
    AHCI_DEV_SATA,
    AHCI_DEV_SEMB,
    AHCI_DEV_PM,
    AHCI_DEV_SATAPI,
} AHCIDeviceType;
extern char* AHCIDeviceTypeName[];

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

typedef volatile struct
{
    U32 clb;       // 0x00, command list base address, 1K-UINT8 aligned
    U32 clbu;      // 0x04, command list base address upper 32 bits
    U32 fb;        // 0x08, FIS base address, 256-UINT8 aligned
    U32 fbu;       // 0x0C, FIS base address upper 32 bits
    U32 is;        // 0x10, interrupt status
    U32 ie;        // 0x14, interrupt enable
    U32 cmd;       // 0x18, command and status
    U32 rsv0;      // 0x1C, Reserved
    U32 tfd;       // 0x20, task file data
    U32 sig;       // 0x24, signature
    U32 ssts;      // 0x28, SATA status (SCR0:SStatus)
    U32 sctl;      // 0x2C, SATA control (SCR2:SControl)
    U32 serr;      // 0x30, SATA error (SCR1:SError)
    U32 sact;      // 0x34, SATA active (SCR3:SActive)
    U32 ci;        // 0x38, command issue
    U32 sntf;      // 0x3C, SATA notification (SCR4:SNotification)
    U32 fbs;       // 0x40, FIS-based switch control
    U32 rsv1[11];  // 0x44 ~ 0x6F, Reserved
    U32 vendor[4]; // 0x70 ~ 0x7F, vendor specific
} __attribute__((packed)) HBA_PORT;

//HBA Memory Registers
typedef volatile struct
{
    U32 cap;        // 0x00, Host capability
    U32 ghc;        // 0x04, Global host control
    U32 is;         // 0x08, Interrupt status
    U32 pi;         // 0x0C, Port implemented
    U32 vs;         // 0x10, Version
    U32 ccc_ctl;    // 0x14, Command completion coalescing control
    U32 ccc_pts;    // 0x18, Command completion coalescing ports
    U32 em_loc;     // 0x1C, Enclosure management location
    U32 em_ctl;     // 0x20, Enclosure management control
    U32 cap2;       // 0x24, Host capabilities extended
    U32 bohc;       // 0x28, BIOS/OS handoff control and status
    U8  rsv[0xA0-0x2C]; 
    // 0xA0 - 0xFF, Vendor specific registers
    U8  vendor[0x100-0xA0];
    // 0x100 - 0x10FF, Port control registers
    HBA_PORT ports[1];// 1 ~ 32
} __attribute__((packed)) HBA_MEM;
typedef struct
{
    // DW0
    U8 cfl:5; // Command FIS length in DWORDS, 2 ~ 16
    U8 a:1; // ATAPI
    U8 w:1; // Write, 1: H2D, 0: D2H
    U8 p:1; // Prefetchable
    U8 r:1; // Reset
    U8 b:1; // BIST
    U8 c:1; // Clear busy upon R_OK
    U8 rsv0:1; // Reserved
    U8 pmp:4; // Port multiplier port
    U16 prdtl; // Physical region descriptor table length in entries
    // DW1
    volatile U32 prdbc; // Physical region descriptor byte count transferred
    // DW2, 3
    U32 ctba; // Command table descriptor base address
    U32 ctbau; // Command table descriptor base address upper 32 bits
    // DW4 - 7
    U32 rsv1[4]; // Reserved
} HBA_CMD_HEADER;
typedef struct 
{
    PCIDevice base;
    block_dev dev;
    HBA_MEM* ahci_bar;
    U32 command_base;
} AHCIDevice;
//定义AHCI设备的设备树格式。
typedef struct
{
    block_dev_node header;
    AHCIDevice controller;
}AHCIDeviceTreeNode;
void sata_ahci_register(U8 bus,U8 slot,U8 func);
#endif
