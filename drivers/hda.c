#include <drivers/hda.h>
#include <common/printk.h>
#include <interrupt/irq.h>
#include <lai/helpers/pci.h>
#include <memory/memory.h>
#include <common/kstring.h>
#define hda_printk(x...) printk(" HDA:" x)
static U8 hda_controller_count=0;
static char* hda_controller_tree_template = "HDAController%d";
static char* hda_codec_tree_template = "HDACodec%d";
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
        printk(" LAI:Cannot find interrupt for HDA controller!\n");
    }
    irq_register(resource.base, 0xDA,0,0,hda_interrupt_handler);
    controller.base.irq = resource.base;
    char buf[DT_NAME_LENGTH_MAX];
    device_tree_node* base_node = device_tree_resolve_by_path(BASE_DEVICE_TREE_TEMPLATE,nullptr,DT_CREATE_IF_NONEXIST);
    HDAControllerTreeNode* controller_node = allocate_page(1);
    memset(controller_node,0,sizeof(HDAControllerTreeNode));
    sprintk(buf,hda_controller_tree_template,hda_controller_count++);
    controller_node->controller = controller;
    strcopy(controller_node->header.name,buf,DT_NAME_LENGTH_MAX);
    controller_node->header.type = DT_BLOCK_DEVICE;
    device_tree_add_from_parent((device_tree_node*)controller_node,(device_tree_node*)base_node);
    int mask=0x01;
    int hda_codec_count=0;
    for(int i=0;i<MAX_CODEC_COUNT;i++) {
        if((controller.registers->statests)&mask) {
            HDACodecTreeNode* codec_node = allocate_page(1);
            memset(codec_node,0,sizeof(HDACodecTreeNode));
            sprintk(buf,hda_codec_tree_template,hda_codec_count++);
            codec_node->codec.controller = &(controller_node->controller);
            controller_node->controller.codecs[i] = &(codec_node->codec);
            codec_node->codec.codec_id = i;
            strcopy(codec_node->header.name,buf,DT_NAME_LENGTH_MAX);
            codec_node->header.type = DT_BLOCK_DEVICE;
            device_tree_add_from_parent((device_tree_node*)codec_node,(device_tree_node*)controller_node);
        }
        mask <<=1;
    }
    if(!hda_codec_count) { //No codec found, rubbish.
        return;
    }
    //Allocate page for CORB and RIRB.
    void* page_corb_rirb = allocate_page(1);
    page_corb_rirb=(void*)((U64)page_corb_rirb & (~KERNEL_ADDR_OFFSET));
    //stop CORB & RIRB.
    controller.registers->corbctl = 0;
    controller.registers->rirbctl = 0;
    //Allocate space for CORB and RIRB.
    char corb_size_accepted = ((controller.registers->corbsize)&0xf0)>>4;
    //From bigger to smaller.
    if(corb_size_accepted&0b0100) {
        //256 entries
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0x10;
    }
    else if(corb_size_accepted&0b0010) {
        //16 entries
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0x01;
    }
    else if(corb_size_accepted&0b0001) {
        //2 entries
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0x00;
    }
    else {
        hda_printk(" Invalid CORB size.\n");
        return;
    }
    controller.registers->corbubase = ((U64)page_corb_rirb)>>32;
    controller.registers->corblbase = ((U64)page_corb_rirb)&0xFFFFFFFF;
    controller.registers->corbrp    = 0x8000;
    controller.registers->corbwp    = 0;
    controller.registers->corbctl   = 0x02;

    char rirb_size_accepted = ((controller.registers->rirbsize)&0xf0)>>4;
    //From bigger to smaller.
    if(rirb_size_accepted&0b0100) {
        //256 entries
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0x10;
    }
    else if(rirb_size_accepted&0b0010) {
        //16 entries
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0x01;
    }
    else if(rirb_size_accepted&0b0001) {
        //2 entries
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0x00;
    }
    else {
        hda_printk(" Invalid RIRB size.\n");
        return;
    }
    controller.registers->rirbubase = ((U64)page_corb_rirb)>>32;
    controller.registers->rirblbase = (((U64)page_corb_rirb)&0xFFFFFFFF)+0x800;
    controller.registers->rirbwp    = 0x8000;
    controller.registers->rirbctl   = 0x03;

    hda_controller_count++;
}

//END
