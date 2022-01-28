#include <drivers/keyboard.h>
#include <common/printk.h>
#include <interrupt/interrupt.h>
#include <interrupt/irq.h>
#include <common/io.h>
#include <drivers/fbcon.h>
static const U8 SCAN_CODE_MAPPING[] = "\x00""\x1B""1234567890-=""\x08""\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0-456+1230.\0\0\0\0\0";
static const U8 SCAN_CODE_MAPPING_SHIFTED[] = "\x00""\x1B""!@#$%^&*()_+""\x08""\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0789-456+1230.\0\0\0\0\0";
U8 keyboard_getchar(struct CharDev*);
void init_keyboard(void)
{
    irq_register(0x01,0x21,0x00,0x00,keyboard_getkey,nullptr);
    g_default_console->common.getchar = keyboard_getchar;
    g_default_console->input_sem = 0;
}
void keyboard_getkey(int no)
{
    static U8 shift=0;
    static U8 caps=0;
    /*
     shift=0,caps=0 小写
     shift=1,caps=1 小写
     shift=0,caps=1 大写
     shift=1,caps=0 小写

     result = shift^caps
     */

    (void)no;
    U8 scancode = inb(0x60);
    if (scancode & KEY_RELEASE) {
        scancode = scancode & ~KEY_RELEASE;
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
           shift = 0;
        }
    }
    else {
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift = 1;
        }
        if (scancode == KEY_CAPS) {
            caps = !caps;
        }
        else {
            char pressed_char;
            if(scancode>=sizeof(SCAN_CODE_MAPPING)) {
                return;
            }
            if (shift^caps) {
                pressed_char = SCAN_CODE_MAPPING_SHIFTED[scancode];
            }
            else {
                pressed_char = SCAN_CODE_MAPPING[scancode];
            }
            queue_in_single(&g_default_console->input_buffer.queue,pressed_char);
            release_semaphore(&g_default_console->input_sem);
        }
    }
}
U8 keyboard_getchar(struct CharDev* dev)
{
    (void)dev;
    acquire_semaphore(&g_default_console->input_sem);
    return queue_out_single(&g_default_console->input_buffer.queue);
}
