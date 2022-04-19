#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <types.h>
#include <coldpoint/common/class.h>
#include <coldpoint/threading/thread.h>
#include <common/printk.h>
//#define print_opcode(x...) printk(x)
#define print_opcode(x...)
extern cpstatus(*g_automata_opcode[256])(thread* t);
void except(thread* t,char* msg);
void automata_main_loop();
extern volatile U8 g_thread_time_expired;
#endif
