#include <kernel.h>
#include <printk.h>
#include <fbcon.h>
#include <memorysetup.h>
#include <acpi_parser.h>

//定义的标准输入/输出。
char_dev stdio;

void kernel_main(KernelInfo info) {
    graphics_init(info.gop);
    graphics_clear_screen(0x001e1e1e);
    fbcon_init(&stdio);
    U32 default_font_color;
    default_font_color = 0x00ffffff;
    printk(" Hello World! I am fzOS.");
    printk("\n Kernel version: fzKernel-0.1.2\n");
    int width=info.gop->Mode->Info->PixelsPerScanLine/8-1;
    for(int i=0;i<width-2;i++)
    {
        printk("+");
    }
    printk("\n Kernel lowest address: %x",info.kernel_lowest_address);
    printk("\n Kernel Memory Usage: %x",info.kernel_page_count);
    U64 memorymappointer;
    memorymappointer = (U64) info.memory_map;
    printk("\n Memory map pointer: %x", memorymappointer);
    printk("\n Memory map size: %x", info.mem_map_size);
    printk("\n Memory Map Descriptor: %x",info.mem_map_descriptor_size);
    memory_init(info.mem_map_descriptor_size,info.mem_map_size,info.memory_map,default_font_color);
    printk("Parsing ACPI table......\n");
    parse_acpi(info.rsdp_address);
}