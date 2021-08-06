#ifndef SATA_AHCI
#define SATA_AHCI
#include <drivers/pci.h>
#include <drivers/blockdev.h>
#include <drivers/devicetree.h>
#include <common/semaphore.h>
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

typedef enum
{
    AHCI_COMMAND_H2D,
    AHCI_COMMAND_D2H
} AHCIOperationType;
typedef enum
{
    AHCI_COMMAND_PREFETCHABLE,
    AHCI_COMMAND_NOT_PREFETCHABLE
} AHCIPrefetchType;
typedef enum
{
    ATA_OPERATION_READ,
    ATA_OPERATION_WRITE
} ATAOperationType;
typedef volatile struct
{
    U32 clb;       // 0x00, command list base address, 1K-U8 aligned
    U32 clbu;      // 0x04, command list base address upper 32 bits
    U32 fb;        // 0x08, FIS base address, 256-U8 aligned
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
    U32 dba;    // Data base address
    U32 dbau;   // Data base address upper 32 bits
    U32 rsv0;   // Reserved
    // DW3
    U32 dbc:22; // Byte count, 4M max
    U32 rsv1:9; // Reserved
    U32 i:1;    // Interrupt on completion
} HBA_PRDT_ENTRY;
typedef struct
{
    // 0x00
    U8 cfis[64]; // Command FIS
    // 0x40
    U8 acmd[16]; // ATAPI command, 12 or 16 bytes
    // 0x50
    U8 rsv[48]; // Reserved
    // 0x80
    HBA_PRDT_ENTRY prdt_entry[1]; // Physical region descriptor table entries, 0 ~
} HBA_CMD_TBL;
typedef struct
{
    // DWORD 0
    U8 fis_type; // FIS_TYPE_REG_H2D
    U8 pmport:4; // Port multiplier
    U8 rsv0:3; // Reserved
    U8 c:1; // 1: Command, 0: Control
    U8 command; // Command register
    U8 featurel; // Feature register, 7:0
    // DWORD 1
    U8 lba0; // LBA low register, 7:0
    U8 lba1; // LBA mid register, 15:8
    U8 lba2; // LBA high register, 23:16
    U8 device; // Device register
    // DWORD 2
    U8 lba3; // LBA register, 31:24
    U8 lba4; // LBA register, 39:32
    U8 lba5; // LBA register, 47:40
    U8 featureh; // Feature register, 15:8
    // DWORD 3
    U8 countl; // Count register, 7:0
    U8 counth; // Count register, 15:8
    U8 icc; // Isochronous command completion
    U8 control; // Control register
    // DWORD 4
    U8 rsv1[4]; // Reserved
} __attribute__((packed)) FIS_REG_H2D;

typedef struct
{
    // DWORD 0
    U8 fis_type; // FIS_TYPE_REG_D2H
    U8 pmport:4; // Port multiplier
    U8 rsv0:2; // Reserved
    U8 i:1; // Interrupt bit
    U8 rsv1:1; // Reserved
    U8 status; // Status register
    U8 error; // Error register
    // DWORD 1
    U8 lba0; // LBA low register, 7:0
    U8 lba1; // LBA mid register, 15:8
    U8 lba2; // LBA high register, 23:16
    U8 device; // Device register
    // DWORD 2
    U8 lba3; // LBA register, 31:24
    U8 lba4; // LBA register, 39:32
    U8 lba5; // LBA register, 47:40
    U8 rsv2; // Reserved
    // DWORD 3
    U8 countl; // Count register, 7:0
    U8 counth; // Count register, 15:8
    U8 rsv3[2]; // Reserved
    // DWORD 4
    U8 rsv4[4]; // Reserved
} __attribute__((packed)) FIS_REG_D2H;
typedef struct 
{
    block_dev dev;
    PCIDevice base;
    HBA_MEM* ahci_bar;
    U32 command_base;
    U32 port_count;
} AHCIController;
//定义AHCI控制器的设备树格式。
typedef struct
{
    device_tree_node header;
    AHCIController controller;
}AHCIControllerTreeNode;
//定义AHCI ATA IDENTIFY的响应格式。
typedef struct {
    U16  config;                                  ///< General Configuration.
    U16  obsolete_1;
    U16  specific_config;                         ///< Specific Configuration.
    U16  obsolete_3;
    U16  retired_4_5[2];
    U16  obsolete_6;
    U16  cfa_reserved_7_8[2];
    U16  retired_9;
    U8   SerialNo[20];                            ///< word 10~19
    U16  retired_20_21[2];
    U16  obsolete_22;
    U8   FirmwareVer[8];                          ///< word 23~26
    U8   ModelName[40];                           ///< word 27~46
    U16  multi_sector_cmd_max_sct_cnt;
    U16  trusted_computing_support;
    U16  capabilities_49;
    U16  capabilities_50;
    U16  obsolete_51_52[2];
    U16  field_validity;
    U16  obsolete_54_58[5];
    U16  multi_sector_setting;
    U16  user_addressable_sectors_lo;
    U16  user_addressable_sectors_hi;
    U16  obsolete_62;
    U16  multi_word_dma_mode;
    U16  advanced_pio_modes;
    U16  min_multi_word_dma_cycle_time;
    U16  rec_multi_word_dma_cycle_time;
    U16  min_pio_cycle_time_without_flow_control;
    U16  min_pio_cycle_time_with_flow_control;
    U16  additional_supported;                    ///< word 69
    U16  reserved_70;
    U16  reserved_71_74[4];                       ///< Reserved for IDENTIFY PACKET DEVICE cmd.
    U16  queue_depth;
    U16  serial_ata_capabilities;
    U16  reserved_77;                             ///< Reserved for Serial ATA
    U16  serial_ata_features_supported;
    U16  serial_ata_features_enabled;
    U16  major_version_no;
    U16  minor_version_no;
    U16  command_set_supported_82;                ///< word 82
    U16  command_set_supported_83;                ///< word 83
    U16  command_set_feature_extn;                ///< word 84
    U16  command_set_feature_enb_85;              ///< word 85
    U16  command_set_feature_enb_86;              ///< word 86
    U16  command_set_feature_default;             ///< word 87
    U16  ultra_dma_mode;                          ///< word 88
    U16  time_for_security_erase_unit;
    U16  time_for_enhanced_security_erase_unit;
    U16  advanced_power_management_level;
    U16  master_password_identifier;
    U16  hardware_configuration_test_result;
    U16  obsolete_94;
    U16  stream_minimum_request_size;
    U16  streaming_transfer_time_for_dma;
    U16  streaming_access_latency_for_dma_and_pio;
    U16  streaming_performance_granularity[2];    ///< word 98~99
    U16  maximum_lba_for_48bit_addressing[4];     ///< word 100~103
    U16  streaming_transfer_time_for_pio;
    U16  max_no_of_512byte_blocks_per_data_set_cmd;
    U16  phy_logic_sector_support;                ///< word 106
    U16  interseek_delay_for_iso7779;
    U16  world_wide_name[4];                      ///< word 108~111
    U16  reserved_for_128bit_wwn_112_115[4];
    U16  reserved_for_technical_report;
    U16  logic_sector_size_lo;                    ///< word 117
    U16  logic_sector_size_hi;                    ///< word 118
    U16  features_and_command_sets_supported_ext; ///< word 119
    U16  features_and_command_sets_enabled_ext;   ///< word 120
    U16  reserved_121_126[6];
    U16  obsolete_127;
    U16  security_status;                         ///< word 128
    U16  vendor_specific_129_159[31];
    U16  cfa_power_mode;                          ///< word 160
    U16  reserved_for_compactflash_161_167[7];
    U16  device_nominal_form_factor;
    U16  is_data_set_cmd_supported;
    U8   additional_product_identifier[8];
    U16  reserved_174_175[2];
    U8   media_serial_number[60];                 ///< word 176~205
    U16  sct_command_transport;                   ///< word 206
    U16  reserved_207_208[2];
    U16  alignment_logic_in_phy_blocks;           ///< word 209
    U16  write_read_verify_sector_count_mode3[2]; ///< word 210~211
    U16  verify_sector_count_mode2[2];
    U16  nv_cache_capabilities;
    U16  nv_cache_size_in_logical_block_lsw;      ///< word 215
    U16  nv_cache_size_in_logical_block_msw;      ///< word 216
    U16  nominal_media_rotation_rate;
    U16  reserved_218;
    U16  nv_cache_options;                        ///< word 219
    U16  write_read_verify_mode;                  ///< word 220
    U16  reserved_221;
    U16  transport_major_revision_number;
    U16  transport_minor_revision_number;
    U16  reserved_224_229[6];
    U64  extended_no_of_addressable_sectors;
    U16  min_number_per_download_microcode_mode3; ///< word 234
    U16  max_number_per_download_microcode_mode3; ///< word 235
    U16  reserved_236_254[19];
    U16  integrity_word;
} __attribute__((packed)) ATA_IDENTIFY_DATA;
//定义AHCI控制器端口的设备树格式。
typedef struct {
    block_dev dev;
    HBA_PORT* port;
    U32 port_no; //为了跳过不存在的端口。
    U8 lba48_enabled;
    U16 sector_size;
    semaphore sem;
    U64 sector_count;
    ATA_IDENTIFY_DATA identify;
} AHCIDevice;
typedef struct
{
    device_tree_node header;
    AHCIDevice device;
    AHCIController* controller;
}AHCIDeviceTreeNode;

//打印一个扇区的内容
#define print_sector(x) \
    for(int i=0;i<512;i++) { \
        if(!(i%16)) { \
            printk("\n"); \
        } \
        printk("%b ",((U8*)x)[i]); \
    }
#endif
