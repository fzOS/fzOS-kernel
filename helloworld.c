#include <kernel.h>
#include <common/printk.h>
#include <drivers/fbcon.h>
#include <memory/memorysetup.h>
#include <memory/memory.h>
#include <common/cpuid.h>
#include <acpi/acpi_parser.h>
#include <common/halt.h>
#include <common/power_control.h>
#include <memory/gdt.h>
#include <interrupt/interrupt.h>
#include <drivers/keyboard.h>
#include <syscall/syscall.h>
#include <drivers/pci.h>
#include <common/kstring.h>
#include <drivers/rtc.h>
#include <filesystem/filesystem.h>
#include <common/file.h>
#include <common/random.h>
#include <coldpoint/classloader.h>
#ifndef VERSION
#define VERSION "0.1"
#endif

//定义的标准输入/输出。
static volatile KernelInfo bss_info;
void kernel_main_real() {
        __asm__("cli");
    graphics_init(bss_info.gop);
    graphics_clear_screen(0x001e1e1e);
    fbcon_init();
    printk("\n Hello World! I am fzOS.\n");
    printk(" Kernel version: %s\n",VERSION);
    int width=bss_info.gop->Mode->Info->PixelsPerScanLine/8-1;
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
    memory_init(bss_info.mem_map_descriptor_size,bss_info.mem_map_size,bss_info.memory_map);
    parse_acpi(bss_info.rsdp_address);
    init_gdt();
    init_interrupt();
    init_device_tree();
    fbcon_add_to_device_tree();
    init_keyboard();
    init_random();
    __asm__("sti");
    init_syscall();
    //然后是PCI设备。
    init_pci();
    //查找根分区并挂载。
    if(mount_root_partition()!=FzOS_SUCCESS) {
        printk(" Error!No root partition found. FzOS cannot continue.\n");
        while(1) {
            halt();
        }
    };

    //显示Banner.
    file banner_file;
    generic_open("/banner",&banner_file);
    void* buf = allocate_page(banner_file.size/PAGE_SIZE+1);
    U64 length =banner_file.filesystem->read(&banner_file,buf,(banner_file.size/PAGE_SIZE+1)*PAGE_SIZE);
    ((U8*)buf)[length] = '\0';
    printk("%s\n",buf);
    free_page(buf,(banner_file.size/PAGE_SIZE+1));
    //启动jvm！
    //init_classloader();
}
void kernel_main(KernelInfo info) {
    //手动换栈。
    bss_info = info;
    bss_info.new_empty_stack+=PAGE_SIZE*KERNEL_STACK_PAGES; //栈反向生长。
    bss_info.new_empty_stack |= KERNEL_ADDR_OFFSET;//保护模式分页
    __asm__(
        "movq %0,%%rsp\n"
        "movq %0,%%rbp\n"
        :
        :"g"(bss_info.new_empty_stack)
        :"memory"
    );
    kernel_main_real();
you_will_never_reach_here:
    halt();
    goto you_will_never_reach_here;
}
