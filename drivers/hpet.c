#include <drivers/hpet.h>
#include <common/printk.h>
#include <interrupt/irq.h>
#include <interrupt/interrupt.h>
HPETResgister* g_hpet_base_address;
#define hpet_debug(x...) printk(x)
void hpet_inthandler(int no)
{
    g_default_console->common.putchar(&g_default_console->common,'A');
    g_default_console->common.flush(&g_default_console->common);
    //g_hpet_base_address->GIntSts |= 0x1;
}
void init_hpet(void)
{
    if(g_hpet_base_address==nullptr) {
        return;
    }
    hpet_debug("HPET Cycle duration: %d fs, Legacy routing %d, %d counter size, %d timers.\n",
               (g_hpet_base_address->GCap)>>32,
               (g_hpet_base_address->GCap&0x8000)>>15,
               ((g_hpet_base_address->GCap&0x2000)>>13)?64:32,
               ((g_hpet_base_address->GCap&0xF00)>>8));
    I64 hpet_cycle_count  = (SYSTEM_TICK_LENGTH_IN_MS*1000000000000) / ((g_hpet_base_address->GCap)>>32);
    printk("It seems that we need %d counts per interrupt.\n",hpet_cycle_count);
    printk("Cap:0x%x\n",g_hpet_base_address->T0Cap);
    g_hpet_base_address->T0Cap |= 0x40;
    g_hpet_base_address->T0Cmp = hpet_cycle_count;
    g_hpet_base_address->T0Cap |= 0xc;
    g_hpet_base_address->GConf &= (~0x3);
    g_hpet_base_address->CntVal = 0;
    g_hpet_base_address->GConf |= 0x2;
    int int_no = get_available_interrupt();
    if(int_no == (int)FzOS_ERROR) {
        printk("Cannot allocate interrupt for HPET.Stop.\n");
        return;
    }
    printk("Register.\n");
    irq_register(2,int_no,0,0,hpet_inthandler,nullptr);
    printk("Register Done.\n");
    printk(" Platform clock switched to HPET.\n");

}
void start_hpet(void)
{
    if(g_hpet_base_address==nullptr) {
        return;
    }
    g_hpet_base_address->GConf &= (~0x3);
    g_hpet_base_address->CntVal = 1234545331;
    g_hpet_base_address->GConf |= 0x3;
}
void stop_hpet(void)
{
    if(g_hpet_base_address==nullptr) {
        return;
    }
    g_hpet_base_address->GConf &= (~0x1);
}
U64 hpet_get_value(void)
{
    if(g_hpet_base_address==nullptr) {
        return 0;
    }
    return g_hpet_base_address->CntVal;
}
