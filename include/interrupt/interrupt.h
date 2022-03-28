#ifndef INTERRUPT_H
#define INTERRUPT_H
#include <types.h>

#define CALL_GATE 0xc
#define INTERRUPT_GATE 0xe
#define TRAP_GATE 0xf
#define MAX_INTERRUPT_COUNT 256
typedef struct {
    U16 target_offset_low;
    U16 target_selector;
    U8 ist : 3;
    U8 reserved1 : 5;
    U8 type : 5;
    U8 dpl : 2;
    U8 present : 1;
    U16 target_offset_middle;
    U32 target_offset_high;
    U32 reserved2;
} __attribute__((packed)) InterruptGateDescriptor;

//中断帧定义。
typedef struct {
    U64 RIP;
    U64 CS;
    U64 RFLAGS;
    U64 RSP;
    U64 SS;
} InterruptFrame;
extern InterruptGateDescriptor g_IDT[256];
void set_interrupt_handler(int index,U64 addr,U8 type);
int get_available_interrupt(void);
void init_interrupt(void);
#endif
