#include <drivers/hda.h>
#include <common/printk.h>
#include <interrupt/irq.h>
#include <lai/helpers/pci.h>
#define hda_printk(x...) printk(" HDA:" x)
void hda_interrupt_handler(int no) {
    hda_printk("Fired from %b\n",no);
}
void hda_register(U8 bus,U8 slot,U8 func) {
    HDAController controller;
    controller.base.bus = bus;
    controller.base.slot = slot;
    controller.base.func = func;
    controller.registers = (void*)((U64)pci_get_bar_address(bus,slot,func,0)|KERNEL_ADDR_OFFSET);

    //Reset controller.
    controller.registers->gctl = (controller.registers->gctl)&0xFFFFFFFE;
    while(controller.registers->gctl&0x00000001) __asm__ volatile("nop");;
    controller.registers->gctl = (controller.registers->gctl)|0x00000001;
    while(!(controller.registers->gctl&0x00000001)) __asm__ volatile("nop");;

    hda_printk("Got Intel HDA v%d.%d device at bus %d,slot %d,func %d.\n",controller.registers->vmaj,controller.registers->vmin,bus,slot,func);
    //尝试获取中断信息。
    union {
        U16 raw;
        U8 split[2];
    } interrupt_info = {
        .raw=pci_config_read_word(bus,slot,func,0x3C)
    };
    acpi_resource_t resource;
    if(lai_pci_route_pin(&resource,0,bus,slot,func,interrupt_info.split[1])!=LAI_ERROR_NONE) {
        printk(" LAI:Cannot find interrupt for ATA controller!\n");
    }
    irq_register(resource.base, 0xDA,0,0,hda_interrupt_handler);
    controller.base.irq = resource.base;
}
