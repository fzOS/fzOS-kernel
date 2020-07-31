#ifndef FADT_H
#define FADT_H
#include <acpi_parser.h>
#include <printk.h>
typedef struct {
    ACPISDTHeader header;
    U32 Reserved0; //FirmwareCtrl,Not used
    U32 Reserved1;         //Dsdt,Not used
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    U8  Reserved2;
 
    U8  PreferredPowerManagementProfile;
    U16 SCI_Interrupt;
    U32 SMI_CommandPort;
    U8  AcpiEnable;
    U8  AcpiDisable;
    U8  S4BIOS_REQ;
    U8  PSTATE_Control;
    U32 PM1aEventBlock;
    U32 PM1bEventBlock;
    U32 PM1aControlBlock;
    U32 PM1bControlBlock;
    U32 PM2ControlBlock;
    U32 PMTimerBlock;
    U32 GPE0Block;
    U32 GPE1Block;
    U8  PM1EventLength;
    U8  PM1ControlLength;
    U8  PM2ControlLength;
    U8  PMTimerLength;
    U8  GPE0Length;
    U8  GPE1Length;
    U8  GPE1Base;
    U8  CStateControl;
    U16 WorstC2Latency;
    U16 WorstC3Latency;
    U16 FlushSize;
    U16 FlushStride;
    U8  DutyOffset;
    U8  DutyWidth;
    U8  DayAlarm;
    U8  MonthAlarm;
    U8  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    U16 BootArchitectureFlags;
 
    U8  Reserved3;
    U32 Flags;
 
    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
 
    U8  ResetValue;
    U8  Reserved4[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    U64 X_FirmwareControl;
    U64 X_Dsdt;
 
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
} __attribute__ ((packed)) FADT;
int parse_fadt(void* in);
#endif