#include <zcrystal/window_console.h>
#include <drivers/fbcon.h>
#include <drivers/keyboard.h>
#include <drivers/asciifont.h>
#include <zcrystal/window_manager.h>
extern Console* g_default_console;
static Console* g_orig_console;
typedef struct {
    Console con;
    U32 character_per_scanline;
    U32 character_line_count;
    int* buffer;
    int current_x,current_y;
    int max_x,max_y;
    int foreground_color,background_color;
    semaphore output_in_progress;
} WindowConsole;
WindowConsole g_window_console = {
    .con.common.getchar = keyboard_getchar,
    .con.common.putchar = window_console_putchar,
    .con.common.flush   = window_console_flush,
    .buffer = nullptr,
    .con.input_buffer.queue.bufsize = BUFFER_MAX,
    .con.output_buffer.queue.bufsize = BUFFER_MAX,
    .output_in_progress = 0,
    .current_x = 0,
    .current_y = 0,
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
inline  U8 char2u8(char c) {
    if ((c >= 'a') && (c <= 'f')) {
        c -= 32;
    }
    if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    }
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }
    else {
        return 255;//不是一个有效的数位。
    }
}
void window_console_move_up()
{
    int delta = (g_window_console.current_y + 16) -g_window_console.max_y;
    int newLine = g_window_console.max_y - delta;
        for (int i = 0; i < g_window_console.max_y; i++){
            for (int j = 0; j < g_window_console.max_x; j++){
                if (i >= newLine){
                    g_window_console.buffer[(i+WINDOW_CAPTION_HEIGHT) * g_window_console.max_x + j] = g_window_console.background_color;
                }else{
                    g_window_console.buffer[(i+WINDOW_CAPTION_HEIGHT) * g_window_console.max_x + j] = g_window_console.buffer[((i+WINDOW_CAPTION_HEIGHT) + delta) * g_window_console.max_x + j];
                }
            }
        }
    g_window_console.current_y = g_window_console.current_y- delta;
}
void window_console_line_break()
{
    g_window_console.current_x = 0;
    g_window_console.current_y +=16;
    if(g_window_console.current_y>g_window_console.max_y) {
        window_console_move_up();
    }
}
void window_console_flush(CharDev* dev)
{
    if(g_window_console.buffer==nullptr) {
        return;
    }
    char c;
    if(g_window_console.output_in_progress) {
        //已经在执行刷新工作;退出。
        return;
    }
    release_semaphore(&g_window_console.output_in_progress);
    while(g_window_console.con.output_sem>0) {
        c = queue_out_single(&g_window_console.con.output_buffer.queue);
        acquire_semaphore(&g_window_console.con.output_sem);
        if(c==LINE_SEPARATOR) {
            window_console_line_break();
            break;
        }
        if(c==COLOR_SWITCH_CHAR) {
            U32 color=0;
            U8 d;
            //切换颜色！
            for(int i=0;i<6;i++) {
                c= queue_out_single(&g_window_console.con.output_buffer.queue);
                acquire_semaphore(&g_window_console.con.output_sem);
                d = char2u8(c);
                if(d==255) {
                    goto normal_print_char;
                }
                color |= (d<<((5-i)*4));
            }
            g_window_console.foreground_color = color;
            continue;
        }
normal_print_char:;
        unsigned const char* dots = FONTDATA_8x16 + c * 16;
        if ((g_window_console.current_x + 8) < g_window_console.max_x) {
            g_window_console.current_x = g_window_console.current_x + 8;
        }
        else {
            g_window_console.current_x = 0;
            g_window_console.current_y = g_window_console.current_y + 16;
        }
        if (g_window_console.max_y < (g_window_console.current_y + 16)) {
            window_console_move_up();
        }
        unsigned char data;	/*定义字符类型变量*/
        int bit;		/*定义bit，用来下方取出8位数组元素中的某一位*/
        int i,j;
        for(j=g_window_console.current_y+WINDOW_CAPTION_HEIGHT;j<g_window_console.current_y+16+WINDOW_CAPTION_HEIGHT;j++)
        {
            data=*dots++;		/*指向下一个字符*/
            bit = 7;			/*从最高位第七位开始*/
            for(i=g_window_console.current_x;i<g_window_console.current_x+8;i++)		/*从高到底显示出8位的每一位*/
            {
                    if(data&(1<<bit)){
                        g_window_console.buffer[j*g_window_console.max_x+i] = g_window_console.foreground_color;	/*调用显示点函数，在此位置显示点*/
                    }
                    bit--;			/*指向低一位*/
            }
        }
    }
    composite();
    acquire_semaphore(&g_window_console.output_in_progress);
}
void hook_window_console(void)
{
    g_orig_console = g_default_console;
    g_default_console = &g_window_console.con;
    ThreadInlineLinkedListNode* node = (ThreadInlineLinkedListNode*)g_thread_list.head.next;
    while(node!=nullptr) {
        if(node->t.console == g_orig_console) {
            node->t.console = &g_window_console.con;
        }
        node = (ThreadInlineLinkedListNode*)node->node.next;
    }
}
void set_window_console_window(Window* w,int fore_color,int back_color)
{
    g_window_console.buffer = (int*)w->buffer.value;
    g_window_console.max_x  = w->width;
    g_window_console.max_y  = w->height;
    g_window_console.background_color = back_color;
    g_window_console.foreground_color = fore_color;
}
void restore_window_console(void)
{
    if(g_orig_console!=nullptr) {
        g_default_console = g_orig_console;
    }
}
