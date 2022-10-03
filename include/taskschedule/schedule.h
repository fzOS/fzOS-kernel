#ifndef TASK_SCHEDULE_H
#define TASK_SCHEDULE_H
#include <types.h>
// define the task status here
#define TASK_DEAD       0
#define TASK_REGISTED   1
#define TASK_ONGOING    2

// item struct of task table
struct g_task_description
{
    U32 uid;
    U8 priority;
    U8 task_status_index;
    U8 status;
    U64 stack_pointer;
};

extern U64 g_trigger_count;
extern U64 g_sche_sys_lock;

void init_task_manager();
void task_schedule();

#endif