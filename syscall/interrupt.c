#include <interrupt.h>
#include <memory.h>
#include <halt.h>
#include <power_control.h>
extern U8 acpi_poweroff_interrupt;
static struct IDTR {
  U16 size;
  U64 address;
} __attribute__((packed)) IDTR;
__attribute__ ((interrupt)) void int_handler_DE (interrupt_frame* frame)
{
    die("Divided by Zero",frame);
}
__attribute__ ((interrupt)) void int_handler_UD (interrupt_frame* frame)
{
    die("Invalid OP Code",frame);
}
__attribute__ ((interrupt)) void int_handler_DF (interrupt_frame* frame)
{
    die("Another Fault in Fault Processing",frame);
}
__attribute__ ((interrupt)) void int_handler_GP (interrupt_frame* frame)
{
    die("General Protection",frame);
}
__attribute__ ((interrupt)) void int_handler_PF (interrupt_frame* frame)
{
    die("Page Fault",frame);
}
interrupt_gate_descriptor IDT[256];
void set_interrupt_handler(int index,U64 addr,U8 type)
{
    memset(IDT+index,0x00,sizeof(interrupt_gate_descriptor));
    IDT[index].target_offset_low = (addr & 0xFFFF);
    IDT[index].target_offset_middle = (addr >> 16) & 0xFFFF;
    IDT[index].target_offset_high = (addr >> 32) & 0xFFFFFFFF;
    IDT[index].target_selector = 0x08;
    IDT[index].type = type;
    IDT[index].present = 1;
}
void init_interrupt(void)
{
    set_interrupt_handler(0,(U64)int_handler_DE,TRAP_GATE);
    set_interrupt_handler(6,(U64)int_handler_UD,TRAP_GATE);    
    set_interrupt_handler(8,(U64)int_handler_DF,TRAP_GATE);   
    set_interrupt_handler(8,(U64)int_handler_GP,TRAP_GATE);   
    set_interrupt_handler(8,(U64)int_handler_PF,TRAP_GATE);   
    IDTR.size = sizeof(IDT)-1;
    IDTR.address = (U64)IDT;
    __asm__ volatile (
        "lidt %0"
        :
        :"m"(IDTR)
    );
}