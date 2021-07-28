#include <drivers/keyboard.h>
#include <common/printk.h>
#include <interrupt/interrupt.h>
#include <interrupt/irq.h>
#include <common/io.h>
void init_keyboard(void)
{
    irq_register(0x01,0x21,0x00,0x00,keyboard_getkey);
}
void keyboard_getkey(int)
{
    printk(" Fired a key:%b.\n",inb(0x60));
}
