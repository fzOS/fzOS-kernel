#include <keyboard.h>
#include <printk.h>
#include <interrupt.h>
#include <irq.h>
#include <io.h>
void init_keyboard(void)
{
    irq_register(0x01,0x21,0x00,0x00,keyboard_getkey);
}
void keyboard_getkey(void)
{
    printk(" Fired a key:%b.\n",inb(0x60));
}