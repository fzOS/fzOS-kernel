#include <drivers/hpet.h>
#include <common/printk.h>
#include <interrupt/irq.h>
#include <interrupt/interrupt.h>
#include <common/halt.h>
#include <coldpoint/automata/automata.h>
HPETResgister* g_hpet_base_address;
void hpet_inthandler(int no)
{
    g_thread_time_expired = 1;
}
void init_hpet(void)
{
    if(g_hpet_base_address==nullptr) {
        return;
    }
    I64 hpet_cycle_count  = (SYSTEM_TICK_LENGTH_IN_MS*1000000000000) / ((g_hpet_base_address->GCap)>>32);
    g_hpet_base_address->T0Cap |= 0x40;
    g_hpet_base_address->T0Cmp = hpet_cycle_count;
    g_hpet_base_address->T0Cap |= 0xc;
    g_hpet_base_address->GConf &= (~0x3);
    g_hpet_base_address->CntVal = 0;
    g_hpet_base_address->GConf |= 0x2;
    int int_no = get_available_interrupt();
    if(int_no == (int)FzOS_ERROR) {
        printk(" Cannot allocate interrupt for HPET.Stop.\n");
        halt();
        return;
    }
    irq_register(2,int_no,0,0,hpet_inthandler,nullptr);
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
