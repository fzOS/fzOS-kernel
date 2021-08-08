#ifndef FBCON
#define FBCON
#include <drivers/console.h>
#include <drivers/graphics.h>
#include <drivers/asciifont.h>
#include <drivers/devicetree.h>
typedef struct {
    console con;
    U64 max_x;
    U64 max_y;
    U64 current_x;
    U64 current_y;
} fbcon;
typedef struct {
    device_tree_node node;
    fbcon con;
}fbcon_device_tree_node;

void fbcon_putchar(char_dev* dev, U8 c);
void fbcon_init(void);
void fbcon_add_to_device_tree(void);
#endif
