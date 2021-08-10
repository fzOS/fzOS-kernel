#include <drivers/keyboard.h>
#include <common/printk.h>
#include <interrupt/interrupt.h>
#include <interrupt/irq.h>
#include <common/io.h>
#include <drivers/fbcon.h>
static U8 SCAN_CODE_MAPPING[] = "\x00""\x1B""1234567890-=""\x08""\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0-456+1230.\0\0\0\0\0";
static U8 SCAN_CODE_MAPPING_SHIFTED[] = "\x00""\x1B""!@#$%^&*()_+""\x08""\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0789-456+1230.\0\0\0\0\0";
U8 keyboard_getchar(struct char_dev*);
void init_keyboard(void)
{
    irq_register(0x01,0x21,0x00,0x00,keyboard_getkey);
    default_console->common.getchar = keyboard_getchar;
    default_console->input_sem = 0;
}
void keyboard_getkey(int no)
{
    static U8 shift=0;
    (void)no;
    U8 scancode = inb(0x60);
    if (scancode & KEY_RELEASE) {
        scancode = scancode & ~KEY_RELEASE;
        if (scancode == 0x2a || scancode == 0x36) {
           shift = 0;
        }
    }
    else {
        if (scancode == 0x2a || scancode == 0x36) {
            shift = 1;
        }
        else {
            char pressed_char;
            if (shift) {
                pressed_char = SCAN_CODE_MAPPING_SHIFTED[scancode];
            }
            else {
                pressed_char = SCAN_CODE_MAPPING[scancode];
            }
            queue_in_single(&default_console->input_buffer.queue,pressed_char);
            release_semaphore(&default_console->input_sem);
        }
    }
}
U8 keyboard_getchar(struct char_dev* dev)
{
    (void)dev;
    acquire_semaphore(&default_console->input_sem);
    return queue_out_single(&default_console->input_buffer.queue);
}
