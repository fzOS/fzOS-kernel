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
#include <common/wav.h>
#include <drivers/hda.h>
#include <filesystem/efivarfs.h>
#ifndef VERSION
#define VERSION "0.1"
#endif

//定义的标准输入/输出。
static volatile KernelInfo g_bss_info;
void print_motd(void);
void show_banner(void);
void play_startup_audio(void);
void print_boot_arg(void);
void kernel_main_real()
{
    __asm__("cli");
    graphics_init(g_bss_info.gop);
    graphics_clear_screen(0x001e1e1e);
    fbcon_init();
    print_motd();
    memory_init(g_bss_info.mem_map_descriptor_size,g_bss_info.mem_map_size,g_bss_info.memory_map);
    parse_acpi(g_bss_info.rsdp_address);
    init_gdt();
    init_interrupt();
    init_device_tree();
    efivarfs_mount(g_bss_info.rt);
    print_boot_arg();
    fbcon_add_to_device_tree();
    init_keyboard();
    init_hpet();
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
    show_banner();
    play_startup_audio();
    //启动jvm！
    init_classloader();
    //print_device_tree();
}
void kernel_main(KernelInfo info)
{
    //手动换栈。
    g_bss_info = info;
    g_bss_info.new_empty_stack+=PAGE_SIZE*KERNEL_STACK_PAGES; //栈反向生长。
    g_bss_info.new_empty_stack |= KERNEL_ADDR_OFFSET;//保护模式分页
    __asm__(
        "movq %0,%%rsp\n"
        "movq %0,%%rbp\n"
        :
        :"g"(g_bss_info.new_empty_stack)
        :"memory"
    );
    kernel_main_real();
you_will_never_reach_here:
    halt();
    goto you_will_never_reach_here;
}
void print_motd(void)
{
    printk("\n Hello World! I am fzOS.\n");
    printk(" Kernel version: %s\n",VERSION);
    int width=g_bss_info.gop->Mode->Info->PixelsPerScanLine/8-1;
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
}
void show_banner(void)
{
    //显示Banner.
    file banner_file;
    generic_open("/banner_color",&banner_file);
    void* buf = allocate_page(banner_file.size/PAGE_SIZE+1);
    U64 length =banner_file.filesystem->read(&banner_file,buf,(banner_file.size/PAGE_SIZE+1)*PAGE_SIZE);
    ((U8*)buf)[length] = '\0';
    printk("%s\n",buf);
    free_page(buf,(banner_file.size/PAGE_SIZE+1));
}
void play_startup_audio(void)
{
    void* buf;
    U64 length;
    file music_file;
    generic_open("/test.wav",&music_file);
    buf = allocate_page(music_file.size/PAGE_SIZE+1);
    length =music_file.filesystem->read(&music_file,buf,(music_file.size/PAGE_SIZE+1)*PAGE_SIZE);
    AudioInfo info;
    if(stat_wav(&info,buf,length)==FzOS_SUCCESS) {
        //FIXME:Use open.
        HDACodecTreeNode* hda_codec_node = (HDACodecTreeNode*)device_tree_resolve_by_path("/Devices/HDAController0/HDACodec0",nullptr,DT_RETURN_IF_NONEXIST);
        if(hda_codec_node==nullptr) { //no sound card
            goto skip_playing_audio;
        }
        play_wav(&info,buf,hda_codec_node->codec.default_output);
    }
    else {
        printk(" Startup Audio not recognized!\n");
    }
skip_playing_audio:
    free_page(buf,(music_file.size/PAGE_SIZE+1));
}
void print_boot_arg(void)
{
    void* buf;
    char file_name[FILENAME_MAX];
    sprintk(file_name,"/EFIVariables/%g-BootParameters",FzOS_VENDOR_GUID);
    file uefi_file;
    U64 length;
    if(generic_open(file_name,&uefi_file)==FzOS_SUCCESS) {
        buf = allocate_page(uefi_file.size/PAGE_SIZE+1);
        memset(buf,0x00,PAGE_SIZE);
        length =uefi_file.filesystem->read(&uefi_file,buf,(uefi_file.size/PAGE_SIZE+1)*PAGE_SIZE);
        if(length!=FzOS_ERROR) {
            printk(" Boot parameters:%s\n",buf);
        }
        free_page(buf,(uefi_file.size/PAGE_SIZE+1));
    }
    else {
        printk(" No boot parameter found. Using default boot procedure.\n");
    }
}
