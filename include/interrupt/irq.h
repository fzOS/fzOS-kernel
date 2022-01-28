#ifndef IRQ
#define IRQ
#include <types.h>
#include <limit.h>
typedef union {
    struct {
    U8 vector;
    U8 delivery_mode:3;
    U8 destination_mode:1;
    U8 delivery_status:1;
    U8 pin_polarity:1;
    U8 remote_irr:1;
    U8 trigger_mode:1;
    U8 mask:1;
    U64 reserved:39;
    U8 destination;
    } __attribute__ ((packed)) split;
    U32 raw[2];
    U64 total;
} IORediectionEntry;
void init_irq(void);
void* get_hardware_by_irq(U8 irq_number);
extern void (*irq_handlers[IRQS_MAX])(int);
extern void (*irq_register)(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer);
extern void (*irq_clear)(void);
#endif
