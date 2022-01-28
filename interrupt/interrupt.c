#include <interrupt/interrupt.h>
#include <memory/memory.h>
#include <common/halt.h>
#include <common/power_control.h>
#include <interrupt/irq.h>
#include <common/printk.h>
extern U8 g_acpi_poweroff_interrupt;
static struct IDTR {
  U16 size;
  U64 address;
} __attribute__((packed)) IDTR;
__attribute__ ((interrupt)) void int_handler_DE (InterruptFrame* frame)
{
    die("Divided by Zero",frame);
}
__attribute__ ((interrupt)) void int_handler_UD (InterruptFrame* frame)
{
    die("Invalid OP Code",frame);
}
__attribute__ ((interrupt)) void int_handler_DF (InterruptFrame* frame)
{
    die("Another Fault in Fault Processing",frame);
}
__attribute__ ((interrupt)) void int_handler_GP (InterruptFrame* frame)
{
    die("General Protection",frame);
}
__attribute__ ((interrupt)) void int_handler_PF (InterruptFrame* frame)
{
    die("Page Fault",frame);
}
__attribute__ ((interrupt)) void int_handler_dummy (InterruptFrame* frame)
{
    debug("Interrupt triggered.\n");
}
//虽然很难看但是还是得写……
//用宏定义会好看一些？
#define irq_handler_reg(x) __attribute__ ((interrupt)) void int_handler_irq_##x (InterruptFrame* frame)\
{\
    irq_handlers[x](x);\
    irq_clear();\
}
irq_handler_reg(0);
irq_handler_reg(1);
irq_handler_reg(2);
irq_handler_reg(3);
irq_handler_reg(4);
irq_handler_reg(5);
irq_handler_reg(6);
irq_handler_reg(7);
irq_handler_reg(8);
irq_handler_reg(9);
irq_handler_reg(10);
irq_handler_reg(11);
irq_handler_reg(12);
irq_handler_reg(13);
irq_handler_reg(14);
irq_handler_reg(15);
irq_handler_reg(16);
irq_handler_reg(17);
irq_handler_reg(18);
irq_handler_reg(19);
irq_handler_reg(20);
irq_handler_reg(21);
irq_handler_reg(22);
irq_handler_reg(23);
void (*g_int_handler_irqs[IRQS_MAX])(InterruptFrame* frame) = {
    int_handler_irq_0,
    int_handler_irq_1,
    int_handler_irq_2,
    int_handler_irq_3,
    int_handler_irq_4,
    int_handler_irq_5,
    int_handler_irq_6,
    int_handler_irq_7,
    int_handler_irq_8,
    int_handler_irq_9,
    int_handler_irq_10,
    int_handler_irq_11,
    int_handler_irq_12,
    int_handler_irq_13,
    int_handler_irq_14,
    int_handler_irq_15,
    int_handler_irq_16,
    int_handler_irq_17,
    int_handler_irq_18,
    int_handler_irq_19,
    int_handler_irq_20,
    int_handler_irq_21,
    int_handler_irq_22,
    int_handler_irq_23
};
//以上。
InterruptGateDescriptor g_IDT[256];
void set_interrupt_handler(int index,U64 addr,U8 type)
{
    memset(g_IDT+index,0x00,sizeof(InterruptGateDescriptor));
    g_IDT[index].target_offset_low = (addr & 0xFFFF);
    g_IDT[index].target_offset_middle = (addr >> 16) & 0xFFFF;
    g_IDT[index].target_offset_high = (addr >> 32) & 0xFFFFFFFF;
    g_IDT[index].target_selector = 0x08;
    g_IDT[index].type = type;
    g_IDT[index].present = 1;
    g_IDT[index].dpl = 3;
}
void init_interrupt(void)
{
    //首先填充一些null的。
    for(int i=0;i<32;i++) {
        set_interrupt_handler(i,(U64)int_handler_dummy,TRAP_GATE);
    }
    set_interrupt_handler(0,(U64)int_handler_DE,TRAP_GATE);
    set_interrupt_handler(6,(U64)int_handler_UD,TRAP_GATE);    
    set_interrupt_handler(8,(U64)int_handler_DF,TRAP_GATE);   
    set_interrupt_handler(13,(U64)int_handler_GP,TRAP_GATE);   
    set_interrupt_handler(14,(U64)int_handler_PF,TRAP_GATE);   
    IDTR.size = sizeof(g_IDT)-1;
    IDTR.address = (U64)g_IDT;
    __asm__ volatile (
        "lidt %0"
        :
        :"m"(IDTR)
    );
    init_irq();
}
