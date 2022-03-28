#ifndef HPET_H
#define HPET_H
#include <types.h>
void init_hpet(void);
void start_hpet(void);
void stop_hpet(void);
typedef struct {
    U64 GCap;//0x00~0x07
    U8 Reserved1[8];//0x08~0x0F
    U64 GConf;//0x10~0x17
    U8 Reserved2[8];//0x18~0x1F
    U64 GIntSts;//0x20~0x27
    U8 Reserved3[200];//0x28~0xEF
    U64 CntVal;//0xF0~0xF7
    U8 Reserved4[8];//0xF8~0xFF
    U64 T0Cap;//0x100~0x107
    U64 T0Cmp;//0x108~0x10F
    U64 T0IRR;//0x110~0x117
    U8  Reserved5[8];//0x118~0x11F
    U64 T1Cap;//0x120~0x127
    U64 T1Cmp;//0x128~0x12F
    U64 T1IRR;//0x130~0x137
    U8  Reserved6[8];//0x138~0x13F
    U64 T2Cap;//0x140~0x147
    U64 T2Cmp;//0x148~0x14F
    U64 T2IRR;//0x150~0x157
    U8  Reserved7[8];//0x158~0x15F
} HPETResgister;
#define SYSTEM_TICK_LENGTH_IN_MS 20
extern HPETResgister* g_hpet_base_address;
#endif
