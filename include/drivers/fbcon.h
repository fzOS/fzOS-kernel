#ifndef FBCON
#define FBCON
#include <drivers/console.h>
#include <drivers/graphics.h>

#include <drivers/devicetree.h>

#define COLOR_SWITCH_CHAR 0x1A
typedef struct {
    console con;
    U64 max_x;
    U64 max_y;
    U64 current_x;
    U64 current_y;
    U32 color;
    semaphore output_in_progress;
} fbcon;
typedef struct {
    device_tree_node node;
    fbcon con;
}fbcon_device_tree_node;

void fbcon_putchar(char_dev* dev, U8 c);
void fbcon_init(void);
void fbcon_set_color(U32 color);
void fbcon_add_to_device_tree(void);
void fbcon_flush(char_dev* dev);
#endif
