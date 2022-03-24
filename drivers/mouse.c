#include <types.h>
#include <drivers/mouse.h>
#include <common/io.h>
#include <common/printk.h>
#include <interrupt/irq.h>
#include <drivers/fbcon.h>
static U8 g_mouse_package_count = 0;
static MouseDataPackage g_mouse_data_package = {.raw={0x00,0x00,0x00}};
static I64 g_mouse_x=0,g_mouse_y=0;
static U8 g_left_button=0,g_right_button=0,g_middle_button=0;
U8 mouse_write(U8 data)
{
    U8 ret;
    outb(0x64,0xD4);//Select mouse.
    outb(0x60,data);//Send command.
    while(!(inb(0x64) & 1)) {
         __asm("pause");
    }
    ret = inb(0x60);
    return ret;
}
void init_mouse(void)
{
    g_mouse_x = g_graphics_data.pixels_per_line/2;
    g_mouse_y = g_graphics_data.pixels_vertical/2;
    mouse_write(0xFF);//Mouse Reset.
    mouse_write(0xF4);//Mouse enable.
    g_mouse_package_count = 2;
    irq_register(0x0c,0x31,0x00,0x00,mouse_getmove,nullptr);
}
void mouse_getmove(int i)
{
    U8 ret = inb(0x60);
    g_mouse_data_package.raw[g_mouse_package_count] = ret;
    if(g_mouse_package_count==2) {
        if(g_mouse_data_package.raw[0]==0) {
            g_mouse_package_count = 0;
            return;
        }
        g_left_button = g_mouse_data_package.data.button_left;
        g_right_button = g_mouse_data_package.data.button_right;
        g_middle_button = g_mouse_data_package.data.button_middle;
        g_mouse_x += (I8)g_mouse_data_package.data.x_offset;
        if(g_mouse_x<0) {
            g_mouse_x = 0;
        }
        else if(g_mouse_x > g_graphics_data.pixels_per_line) {
            g_mouse_x = g_graphics_data.pixels_per_line;
        }
        g_mouse_y -= (I8)g_mouse_data_package.data.y_offset;//Why do they invert Y?
        if(g_mouse_y<0) {
            g_mouse_y = 0;
        }
        else if(g_mouse_y > g_graphics_data.pixels_vertical) {
            g_mouse_y = g_graphics_data.pixels_vertical;
        }
    }
    g_mouse_package_count = (g_mouse_package_count+1)%3;
    (void)i;
    (void)g_mouse_x;
    (void)g_mouse_y;

}
