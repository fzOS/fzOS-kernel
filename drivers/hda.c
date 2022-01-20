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
static char* hda_connector_type[] = {
    "LineOut",
    "Speaker",
    "HPOut",
    "CD",
    "SPDIFOut",
    "DigitalOut",
    "ModemLine",
    "ModemHandset",
    "LineIn",
    "Aux",
    "MicIn",
    "Telephony",
    "SPDIFIn",
    "DigitalIn",
    "Reserved",
    "Unknown"
};
void hda_interrupt_handler(int no) {
    hda_printk("Fired from %b\n",no);
}
U32 hda_execute_verb(HDAController* controller,U32 verb)
{
    //First,check if there is room in CORB.
    int next_corb_pos = (controller->registers->corbwp +1)%controller->corb_count;
    int prev_rirb_pos = (controller->registers->rirbwp);
    while(next_corb_pos == controller->registers->corbrp) __asm__ volatile("pause");
    //Then,put the verb into corb;
    controller->corb[next_corb_pos] = verb;
    controller->registers->corbwp = next_corb_pos;
    while(prev_rirb_pos == controller->registers->rirbwp) __asm__ volatile("pause");
    return (U32)(controller->rirb[controller->registers->rirbwp]);
}
int set_widget_power_state(HDACodec* codec,int widget_id,int power_state)
{
    HDAVerb verb;
    verb.packed = 0;
    verb.split.command=CODEC_SET_POWER_STATE;
    verb.split.codec_addr = codec->codec_id;
    verb.split.node_id = widget_id;
    verb.split.data = power_state;
    U32 ret = hda_execute_verb(codec->controller,verb.packed);
    if(ret!=0) {
        return ret;
    }
    verb.split.command=CODEC_GET_POWER_STATE;
    verb.split.data = 0;
    ret = hda_execute_verb(codec->controller,verb.packed);
    return FzOS_SUCCESS;
}
//END
void hda_register(U8 bus,U8 slot,U8 func) {
    HDAController controller;
    controller.base.bus = bus;
    controller.base.slot = slot;
    controller.base.func = func;
    controller.registers = (void*)((U64)pci_get_bar_address(bus,slot,func,0)|KERNEL_ADDR_OFFSET);

    //Reset controller.
    controller.registers->gctl = (controller.registers->gctl)&0xFFFFFFFE;
    while(controller.registers->gctl&0x00000001) __asm__ volatile("pause");
    controller.registers->gctl = (controller.registers->gctl)|0x00000001;
    while(!(controller.registers->gctl&0x00000001)) __asm__ volatile("pause");
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
    strcopy(controller_node->header.name,buf,DT_NAME_LENGTH_MAX);
    controller_node->header.type = DT_BLOCK_DEVICE;
    device_tree_add_from_parent((device_tree_node*)controller_node,(device_tree_node*)base_node);
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
        controller.corb_count = 256;
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0b10;
    }
    else if(corb_size_accepted&0b0010) {
        //16 entries
        controller.corb_count = 16;
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0b01;
    }
    else if(corb_size_accepted&0b0001) {
        //2 entries
        controller.corb_count = 2;
        controller.registers->corbsize = ((controller.registers->corbsize)&0xf0) | 0b00;
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
        controller.rirb_count = 256;
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0b10;
    }
    else if(rirb_size_accepted&0b0010) {
        //16 entries
        controller.rirb_count = 16;
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0b01;
    }
    else if(rirb_size_accepted&0b0001) {
        //2 entries
        controller.rirb_count = 2;
        controller.registers->rirbsize = ((controller.registers->rirbsize)&0xf0) | 0b00;
    }
    else {
        hda_printk(" Invalid RIRB size.\n");
        return;
    }
    controller.registers->rirbubase = ((U64)page_corb_rirb)>>32;
    controller.registers->rirblbase = (((U64)page_corb_rirb)&0xFFFFFFFF)+0x800;
    controller.registers->rirbwp    = 0x8000;
    controller.registers->rirbctl   = 0x03;
    controller.corb = page_corb_rirb;
    controller.rirb = (U64*)((U64)page_corb_rirb+0x800);
    controller.buffer_desciptor_list = allocate_page(1);
    memset(controller.buffer_desciptor_list,0x00,PAGE_SIZE);
    controller_node->controller = controller;
    int mask=0x01;
    int hda_codec_count=0;
    HDAVerb verb;
    U8 hda_connector_type_count[sizeof(hda_connector_type)/sizeof(char*)];
    char hda_connector_name_with_number[DT_NAME_LENGTH_MAX];
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

            //Get Dev/Ven/Sub.
            verb.split.codec_addr = i;
            verb.split.command    = CODEC_GET_PARAMETER;
            verb.split.data       = PARAM_VEN_DEV_ID;
            verb.split.node_id    = 0;
            int ret = hda_execute_verb(&controller,verb.packed);
            hda_printk("Codec #%d:PID:%w,VID:%w.\n",i,(ret&0xFFFF0000)>>16,ret&0xFFFF);
            //Get Func Group.
            verb.split.data       = PARAM_NODE_COUNT;
            ret = hda_execute_verb(&controller,verb.packed);
            int starting_node_number = (ret&0xFF0000)>>16;
            int node_count = (ret&0xFF);
            codec_node->codec.afg_id = starting_node_number;
            //Power On AFG node.
            set_widget_power_state(&codec_node->codec,starting_node_number,0);
            for(int j=0;j<node_count;j++) {
                controller.afg_nodes[j+starting_node_number]=0;
                verb.split.command    = CODEC_GET_PARAMETER;
                verb.split.data       = PARAM_FUNC_GROUP_TYPE;
                verb.split.node_id    = j+starting_node_number;
                ret = hda_execute_verb(&controller,verb.packed);
                if((ret&0xFF)==0x01) {
                    verb.split.data       = PARAM_NODE_COUNT;
                    verb.split.node_id    = j+starting_node_number;
                    ret = hda_execute_verb(&controller,verb.packed);
                    int widget_start = (ret&0xFF0000)>>16;
                    int widget_count = (ret&0xFF);
                    AudioWidgetCap audio_widget_cap;
                    //Output,Input,Mixer,Selector,Pin Complex,Power Widget
                    U8 output_count=0,input_count=0,mixer_count=0,selector_count=0,pin_complex_count=0,power_widget_count=0;
                    for(int k=0;k<widget_count;k++) {
                        verb.split.data       = PARAM_AUDIO_WIDGET_CAP;
                        verb.split.node_id    = k+widget_start;
                        audio_widget_cap.raw = hda_execute_verb(&controller,verb.packed);
                        switch(audio_widget_cap.split.type) {
                            case 0x00: {output_count++;break;}
                            case 0x01: {input_count++;break;}
                            case 0x02: {mixer_count++;break;}
                            case 0x03: {selector_count++;break;}
                            case 0x04: {pin_complex_count++;break;}
                            case 0x05: {power_widget_count++;break;}
                            default:break;
                        }
                    }
                    hda_printk("Codec #%d:%d outputs,%d inputs,%d mixers,%d selectors,%d pin complices,%d power widgets\n",i,
                        output_count,
                        input_count,
                        mixer_count,
                        selector_count,
                        pin_complex_count,
                        power_widget_count
                    );
                    codec_node->codec.output_index      = 0;
                    codec_node->codec.input_index       = codec_node->codec.output_index+output_count;
                    codec_node->codec.mixer_index       = codec_node->codec.input_index+input_count;
                    codec_node->codec.selector_index    = codec_node->codec.mixer_index+mixer_count;
                    codec_node->codec.pin_complex_index = codec_node->codec.selector_index+selector_count;
                    codec_node->codec.power_widget_index= codec_node->codec.pin_complex_index+pin_complex_count;
                    //2nd iteration:put into array.
                    output_count = input_count = mixer_count = selector_count = pin_complex_count = power_widget_count = 0;
                    for(int k=0;k<widget_count;k++) {
                        verb.split.data       = PARAM_AUDIO_WIDGET_CAP;
                        verb.split.node_id    = k+widget_start;
                        audio_widget_cap.raw = hda_execute_verb(&controller,verb.packed);
                        switch(audio_widget_cap.split.type) {
                            case 0x00: {
                                codec_node->codec.audio_widgets[codec_node->codec.output_index+output_count] = k+widget_start;
                                output_count++;
                                break;
                            }
                            case 0x01: {
                                codec_node->codec.audio_widgets[codec_node->codec.input_index+input_count] = k+widget_start;
                                input_count++;
                                break;
                            }
                            case 0x02:  {
                                codec_node->codec.audio_widgets[codec_node->codec.mixer_index+mixer_count] = k+widget_start;
                                mixer_count++;
                                break;
                            }
                            case 0x03: {
                                codec_node->codec.audio_widgets[codec_node->codec.selector_index+selector_count] = k+widget_start;
                                selector_count++;
                                break;
                            }
                            case 0x04: {
                                codec_node->codec.audio_widgets[codec_node->codec.pin_complex_index+pin_complex_count] = k+widget_start;
                                pin_complex_count++;
                                break;
                            }
                            case 0x05: {
                                codec_node->codec.audio_widgets[codec_node->codec.power_widget_index+power_widget_count] = k+widget_start;
                                power_widget_count++;
                                break;
                            }
                            default: {break;}
                        }
                    }

                    //We only cares about outputs.
                    for(int i=0;i<sizeof(hda_connector_type_count);i++) {
                        hda_connector_type_count[i]=0;
                    }
                    HDAConfigurationDefault conf_default;
                    for(int k=0;k<pin_complex_count;k++) {
                        verb.split.command = CODEC_GET_CONF_DEFAULT;
                        verb.split.data    = 0;
                        verb.split.node_id = codec_node->codec.audio_widgets[k+codec_node->codec.pin_complex_index];
                        conf_default.packed = hda_execute_verb(&controller,verb.packed);
                        if(conf_default.packed!=0) {
                            //Create Tree Node.
                            HDAConnectorTreeNode* conn_node = allocate_page(1);
                            memset(conn_node,0x00,PAGE_SIZE);
                            if(hda_connector_type_count[conf_default.split.def_device]==0) {
                                strcopy(conn_node->header.name,hda_connector_type[conf_default.split.def_device],DT_NAME_LENGTH_MAX);
                            }
                            else {
                                sprintk(hda_connector_name_with_number,"%s%d",hda_connector_type[conf_default.split.def_device],hda_connector_type_count[conf_default.split.def_device]+1);
                                strcopy(conn_node->header.name,hda_connector_name_with_number,DT_NAME_LENGTH_MAX);
                            }
                            hda_connector_type_count[conf_default.split.def_device]++;
                            conn_node->connector.codec = &(codec_node->codec);
                            conn_node->connector.widget_id = verb.split.node_id;
                            conn_node->connector.connector_name = conn_node->header.name;
                            conn_node->header.type=DT_BLOCK_DEVICE;
                            conn_node->connector.pin_default.packed = conf_default.packed;
                            conn_node->connector.io_direction = ((conf_default.split.def_device&0x02)==0x02);
                            device_tree_add_from_parent((device_tree_node*)conn_node,(device_tree_node*)codec_node);
                            //by default, we choose Speaker/line-out as desired output,mic/line-in as desired input.
                            if(codec_node->codec.default_output==nullptr) {
                                if(conf_default.split.def_device==0x00||conf_default.split.def_device==0x01) {
                                    codec_node->codec.default_output=&(conn_node->connector);
                                }
                            }
                            if(codec_node->codec.default_input==nullptr) {
                                if(conf_default.split.def_device==0x08||conf_default.split.def_device==0x0a) {
                                    codec_node->codec.default_input=&(conn_node->connector);
                                }
                            }
                        }
                    }
                    hda_printk("Codec #%d:Selected %s as default output,%s as default input.\n",i,
                        codec_node->codec.default_output->connector_name,
                        codec_node->codec.default_input->connector_name
                    );
                    //Get ADC/DAC for default output/input.
                    verb.split.command = WIDGET_GET_CONNECTION_ENTRY;
                    verb.split.node_id = codec_node->codec.default_output->widget_id;
                    verb.split.data    = 0;
                    ret = hda_execute_verb(&controller,verb.packed);
                    codec_node->codec.default_output->connected_converter_id = ret;
                    verb.split.node_id = codec_node->codec.default_input->widget_id;
                    ret = hda_execute_verb(&controller,verb.packed);
                    codec_node->codec.default_input->connected_converter_id = ret;
                    //Enable Default output.
                    verb.split.command    = CODEC_GET_PIN_WIDGET_CONTROL;
                    verb.split.data       = 0;
                    U32 pin_status = hda_execute_verb(&controller,verb.packed);
                    verb.split.command    = CODEC_SET_PIN_WIDGET_CONTROL;
                    verb.split.data       = (pin_status&0xFF)|0x40;
                    hda_execute_verb(&controller,verb.packed);
                 }
            }
        }
        mask <<=1;
    }
    if(!hda_codec_count) { //No codec found, rubbish.
        return;
    }
    hda_controller_count++;
}
StreamDescRegisters* get_input_stream_desc(HDAController* controller,int* stream_id_buffer)
{
    //FIXME:return something other than 0
    int chosen=0;
    *stream_id_buffer = chosen;
    return ((void*)controller->registers)+0x80+chosen*sizeof(StreamDescRegisters);
}
StreamDescRegisters* get_output_stream_desc(HDAController* controller,int* stream_id_buffer)
{
    //FIXME:return something other than 0
    int chosen=0;
    *stream_id_buffer = (chosen+((controller->registers->gcap&0xf00)>>8));
    return ((void*)controller->registers)+0x80+(chosen+((controller->registers->gcap&0xf00)>>8))*sizeof(StreamDescRegisters);
}

int bind_stream_to_converter(HDACodec* codec,int stream_id,int converter_widget_id)
{
    printk("Binding %b to %b\n",stream_id,converter_widget_id);
    HDAVerb verb;
    verb.packed = 0;
    verb.split.command    = CODEC_SET_STREAM_CHANNEL;
    verb.split.codec_addr = codec->codec_id;
    verb.split.node_id    = converter_widget_id;
    verb.split.data       = (stream_id&0xF)<<4;
    U32 ret = hda_execute_verb(codec->controller,verb.packed);

    return ret;
}

