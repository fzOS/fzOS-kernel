#include <common/random.h>
#include <common/cpuid.h>
#include <common/printk.h>
#include <drivers/devicetree.h>
#include <drivers/rtc.h>
#include <memory/memory.h>
typedef struct {
    DeviceTreeNode node;
    char_dev dev;
} random_device_tree_node;
static int use_software_random = 0;
static U64 software_random_next = 1;
U8 random_getchar(char_dev* dev)
{
    (void)dev;
    return random_get_u8();
}

int init_random(void)
{
    if(get_processor_rdseed_support()!=FzOS_SUCCESS) {
        printk(" random:RDSEED not supported by processor!Fallback to software mode.\n");
        use_software_random=1;
        //使用RTC时间作为种子。
        RTCTime time;
        read_rtc(&time);
        software_random_next = (time.century*1000+time.day*100+time.month*10+time.hour*20+time.minute*3);
    }
    //在这里初始化一个设备树结点……？
    random_device_tree_node* random_node = allocate_page(1);
    memset(random_node,0,sizeof(random_device_tree_node));
    random_node->node.type=DT_CHAR_DEVICE;
    memcpy(&(random_node->node.name),"RandomGenerator",DT_NAME_LENGTH_MAX);
    random_node->dev.getchar=random_getchar;
    DeviceTreeNode* parent = device_tree_resolve_by_path("/Devices",nullptr,DT_CREATE_IF_NONEXIST);
    device_tree_add_from_parent(&(random_node->node),parent);
    return FzOS_SUCCESS;
}

U8 random_get_u8(void)
{
    return (U8)random_get_u64();
}
U16 random_get_u16(void)
{
    return (U16)random_get_u64();
}
U32 random_get_u32(void)
{
    return (U32)random_get_u64();
}
U64 random_get_u64(void)
{
    U64 ret=0;
    if(!use_software_random) {
         __asm__ goto (
            "movq $100,%%rcx\n"
            "_retry%=:\n"
            "rdseed %%rax\n"
            "jc _done%=\n"      // 获取随机数成功时，CF被置1
            "loopq _retry%=\n"
            "_fail%=:\n"
            "jmp %l[software]\n"
            "_done%=:\n"
            "movq %%rax,%0\n"
            :"=g"(ret)
            : /* no input*/
            :"%rax","%rbx","%rcx","memory"
            :software
        );
    }
    else {
software:
        software_random_next = software_random_next * 25214903917 + 11;
        ret |= ((software_random_next & 0xFFFFFFFF0000)>>16);
        software_random_next = software_random_next * 25214903917 + 11;
        ret |= ((software_random_next & 0xFFFFFFFF0000)<<16);
    }
    return ret;
}
