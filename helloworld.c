#include <kernel.h>
#include <printk.h>
#include <fbcon.h>
#include <memorysetup.h>
#include <cpuid.h>
#include <acpi_parser.h>
#include <halt.h>
#include <power_control.h>
#include <gdt.h>
#include <interrupt.h>
#include <keyboard.h>
#include <syscall.h>
#include <pci.h>

#ifndef VERSION
#define VERSION "0.1"
#endif
//定义的标准输入/输出。
char_dev stdio;

void kernel_main(KernelInfo info) {
    __asm__("cli");
    graphics_init(info.gop);
    graphics_clear_screen(0x001e1e1e);
    fbcon_init(&stdio);
    printk("\n Hello World! I am fzOS.\n");
    printk(" Kernel version: %s\n",VERSION);
    debug(" Kernel begins at %x.\n",kernel_main);
    int width=info.gop->Mode->Info->PixelsPerScanLine/8-1;
    for(int i=0;i<width-2;i++)
    {
        printk("+");
    }
    char buff[100];
    get_processor_vendor(buff);
    buff[12]=0;
    printk("\n CPU information: %s ",buff);
    get_processor_name(buff);
    printk("%s\n",buff);
    memory_init(info.mem_map_descriptor_size,info.mem_map_size,info.memory_map);
    printk(" Parsing ACPI table......\n");
    parse_acpi(info.rsdp_address);
    init_gdt();
    init_interrupt();
    
    
    init_keyboard();
    __asm__("sti");
    //reset();
    //poweroff();
    init_syscall();
    //然后是PCI设备。
    init_pci();
you_will_never_reach_here:
    halt();
    goto you_will_never_reach_here;
}
