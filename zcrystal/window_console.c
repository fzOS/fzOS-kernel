#include <zcrystal/window_console.h>
#include <drivers/fbcon.h>
#include <drivers/keyboard.h>
extern Console* g_default_console;
static Console* g_orig_console;
typedef struct {
    Console con;
    U32 character_per_scanline;
    U32 character_line_count;
} WindowConsole;
WindowConsole g_window_console = {
    .con.common.getchar = keyboard_getchar,
    .con.common.putchar = window_console_putchar,
    .con.common.flush   = window_console_flush
};

void window_console_putchar(CharDev* dev, U8 c)
{
    (void) dev;
    queue_in_single(&g_window_console.con.output_buffer.queue,c);
    release_semaphore(&g_window_console.con.output_sem);
    if(c==LINE_SEPARATOR||g_window_console.con.output_sem>g_window_console.con.output_buffer.queue.bufsize) {
        window_console_flush(dev);
    }
}
void window_console_flush(CharDev* dev)
{
    //TODO:FLUSH!
}
void hook_window_console(void)
{
    g_orig_console = g_default_console;
    g_default_console = &g_window_console.con;
}

void restore_window_console(void)
{
    if(g_orig_console!=nullptr) {
        g_default_console = g_orig_console;
    }
}
