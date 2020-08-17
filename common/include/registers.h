#ifndef REGISTERS
#define REGISTERS
#include "types.h"
//定义一些神奇的结构体。
typedef union {
    struct {
    int padding0:3;//0~2
    int PWT:1;//3
    int PCD:1;//4
    int padding1:7;//5~11
    U64 base_addr:40;//12~51
    int padding2:12;//52~63
    } __attribute__ ((packed)) split;
    U64 raw;
} CR3;
typedef union {
    struct {
        int PE:1;//0,Protection Enabled
        int MP:1;//1,Monitor Coprocessor
        int EM:1;//2,Emulation
        int TS:1;//3,Task Switched
        int ET:1;//4,Extension Type
        int NE:1;//5,Numeric Error
        int padding0:10;//6~15
        int WP:1;//16,Write Protect
        int padding1:1;//17
        int AM:1;//18,Alignment Mask
        int padding2:10;//19~28
        int NW:1;//29,Not Write-through
        int CD:1;//30,Cache Disable
        int PG:1;//31,Paging
    } __attribute__ ((packed)) split;
    U64 raw;
} CR0;
typedef union {
    struct {
        int VME:1;//0,Virtual-8086 Mode Extensions
        int PVI:1;//1,Protected-Mode Virtual Interrupts
        int TSD:1;//2,Time Stamp Disable
        int DE:1; //3,Debugging Extensions
        int PSE:1;//4,Page Size Extensions
        int PAE:1;//5,Physical-Address Extensions
        int MCE:1;//6,Machine Check Enable
        int PGE:1;//7,Page-Global Enable
        int PCE:1;//8,Performance-Monitoring Counter Enable
        int OSFXSR:1;//9,Operating-System FXSAVE/FXRSOTR Support
        int OSXMMEXCPT:1;//10,Operating-System Unmasked Exception Support
        int UMIP:1;//11,User Mode Instruction Prevention
        int padding0:4;//12~15
        int FSGSBASE:1;//16, RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE enabler
        int padding1:1;//17
        int OSXSAVE:1 ;//18,XSAVE,Processor Extended States Enabler
        int padding2:1;//19
        int SMEP:1;//20,Supervisor Mode Execution Prevention
        int SMAP:1;//21.Supervisor Mode Access Protection
        U64 padding3:42;//63-22
    } __attribute__ ((packed)) split;
    U64 raw;
} CR4;
typedef union {
    struct {
        int SCE:1;//0,System Call Extensions
        int padding0:7;//1~7
        int LME:1;//8,Long Mode Enable
        int padding1:1;//9
        int LMA:1;//10,Long Mode Active
        int NXE:1;//11,No-Execute Enable
        int SVME:1;//12,Secure Virtual Machine Enable
        int LMSLE:1;//13,Long Mode Segment Limit Enable
        int FFXSR:1;//14,Fast FXSAVE/FXRSTOR
        int TCE:1;  //15,Translation Cache Extension
        U64 padding2:48;//16~63
    } __attribute__ ((packed)) split;
    U64 raw;
} EFER;
typedef union {
    struct {
        int CF:1;//0,Carry Flag
        int padding0:1;//1
        int PF:1;//2,Parity Flag
        int padding1:1;//3
        int AF:1;//4,Auxiliary Flag
        int padding2:1;//5
        int ZF:1;//6,Zero Flag
        int SF:1;//7,Sign Flag
        int TF:1;//8,Trap Flag
        int IF:1;//9,Interrupt Flag
        int DF:1;//10,Direction Flag
        int OF:1;//11,Overflow Flag
        int IOPL:2;//12~13,IP Privilege Level
        int NT:1;//14,Nested Task
        int padding3:1;//15
        int RF:1;//16,Resume Flag
        int VM:1;//17,Virtual-8086 Mode
        int AC:1;//18,Alignment Clock
        int VIF:1;//19,Virtual Interrupt Flag
        int VIP:1;//20,Virtual Interrupt Pending
        int ID:1; //21,ID Flag
        U64 padding4:42;//22~63
    } __attribute__ ((packed)) split;
    U64 raw;
} RFLAGS;
U64 rdmsr(U64 opcode);
void wrmsr(U64 opcode,U64 value);
#endif