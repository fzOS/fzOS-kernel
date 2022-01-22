#include <interrupt/interrupt.h>
#include <interrupt/irq.h>
#include <common/power_control.h>
#include <common/printk.h>
#include <common/registers.h>
#include <acpi/acpi_parser.h>
#include <acpi/fadt.h>
extern U64 ioapic_address;
extern U64 localapic_address;
extern U8 acpi_interrupt;
extern void (*int_handler_irqs[IRQS_MAX])(interrupt_frame* frame);
void (*irq_handlers[IRQS_MAX])(int);
void (*irq_register)(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer);
void (*irq_clear)(void);
void irq_register_ioapic(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer);
void irq_register_8259(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer);
void irq_clear_ioapic(void);
void irq_clear_8259(void);
//Get buffer pointer by IRQ.
void* hardware_buffer[IRQS_MAX];
void write_ioapic_register(U8 offset, U32 val)
{
    *(volatile U32*)(ioapic_address) = offset;
    *(volatile U32*)(ioapic_address + 0x10) = val;
}
U32 read_ioapic_register(U8 offset)
{
    *(volatile U32*)(ioapic_address) = offset;
    return *(volatile U32*)(ioapic_address + 0x10);
}
void init_irq(void)
{
    if (ioapic_address == 0xFFFFFFFF) {
        irq_register = irq_register_8259;
        irq_clear    = irq_clear_8259;
    } else {
        irq_register = irq_register_ioapic;
        irq_clear    = irq_clear_ioapic;
        //关闭8259.
        outb(0x20,0x11);
        outb(0xA0,0x11);
        outb(0x21, 0x21);
        outb(0xA1, 0x28);
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0xFF);
        outb(0xA1, 0xFF);
        //打开IOAPIC.
        outb(0x22,0x70);
        outb(0x23,0x01);
        //设置local APIC以便启用APIC。
        (*(U32*)(localapic_address+0xF0))=0x1FF;
        U64 apic_en = rdmsr(0x1B);
        apic_en |= 0x800;
        wrmsr(0x1B,apic_en);
    }
    //由于肯定有ACPI，我们在这里注册ACPI关机信号。
    acpi_enable_power_button();
    irq_register(acpi_interrupt, 0xAC,1,1,acpi_interrupt_handler,nullptr);
}

void* get_hardware_by_irq(U8 irq_number)
{
    return hardware_buffer[irq_number];
}
void irq_register_ioapic(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer)
{
    U32 begin_offset = (0x10 + 2 * irq_number);
    io_rediection_entry entry;
    entry.raw[0] = read_ioapic_register(begin_offset);
    entry.raw[1] = read_ioapic_register(begin_offset + 1);
    entry.split.vector = desired_int_no;
    entry.split.mask = 0;
    entry.split.trigger_mode = trigger_mode;
    entry.split.pin_polarity = pin_polarity;
    write_ioapic_register(begin_offset, entry.raw[0]);
    write_ioapic_register(begin_offset + 1, entry.raw[1]);
    set_interrupt_handler(desired_int_no, (U64)(int_handler_irqs[irq_number]), INTERRUPT_GATE);
    irq_handlers[irq_number] = handler;
    hardware_buffer[irq_number] = additional_buffer;
}
void irq_register_8259(U8 irq_number, U8 desired_int_no,U8 trigger_mode,U8 pin_polarity, void (*handler)(int),void* additional_buffer)
{
    debug(" Not implemented.\n");
}
void irq_clear_ioapic(void)
{
    //Test.
    (*(U32*)(localapic_address+0xB0))=0x00;
}
void irq_clear_8259(void)
{
    debug(" Not implemented.\n");    
}
