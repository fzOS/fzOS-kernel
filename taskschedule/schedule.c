// 进程管理器 process manager
// schedule the tasks

#include <interrupt/irq.h>
#include <interrupt/interrupt.h>
#include <taskschedule/schedule.h>
#include <taskschedule/process.h>
#include <types.h>
#include <common/registers.h>
#include <drivers/fbcon.h>
#include <common/kstring.h>

// schedule process: 
/* Interrupt from hpet -- trigger the context switching function to check the current task status -- with in time frame: exit the interrupt
                                            |-- out of the time frame: save the cpu status from IST table -- enter the schedule process (last kernel process)
        -- if enter schedule system: stop the hpet and setup new task function
Loading sequence: setup hpet interrupt, setup schedule system (last function) [idle state], run function/software loader, start hpet
*/

// temp counter for checking the trigger of the schedule system
U64 g_trigger_count;
// locker to let context switching system know the schedule system is working, quit asap 
U64 g_sche_sys_lock;

// initial the task manager
void init_task_manager()
{
    /*
    // allocate the interrupt slot for task schedule 
    int int_no = get_available_interrupt();
    if(int_no == (int)FzOS_ERROR) {
        printk("Cannot allocate interrupt for Task Schedule Module. Stop.\n");
        return;
    }
    set_interrupt_handler(int_no, (U64)(task_schedule), TRAP_GATE);
    // enable APIC timer and set the target vector to the desired interrupt handler number
    // LVT timer address: FEE0 0320H
    U32 apic_timer_register = 0x20000 | int_no;
    U32 apic_timer_counter = ;
    rdmsr(0xFFE00380, );
    wrmsr(0xFFE00320, (U64)apic_timer_register);
    */ 
   // No initial for apic timer here, just use the hpet timer to trigger management;
   g_trigger_count = 0;
}

// the function set into interrrupt trap for schedule the tasks
// use time based to trigger this one
// do context change in this function too
// perform ring change here
void task_schedule()
{
    g_trigger_count += 1;
    if (g_trigger_count%100 == 0)
    {
        printk("schedule change triggered, counter #%d \n", g_trigger_count/100);
    }
    // try copy the IST table here
}