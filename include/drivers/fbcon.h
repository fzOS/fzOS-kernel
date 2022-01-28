#ifndef FBCON
#define FBCON
#include <drivers/console.h>
#include <drivers/graphics.h>

#include <drivers/devicetree.h>

#define COLOR_SWITCH_CHAR 0x1A
typedef struct {
    Console con;
    U64 max_x;
    U64 max_y;
    U64 current_x;
    U64 current_y;
    U32 color;
    semaphore output_in_progress;
} FBCon;
typedef struct {
    DeviceTreeNode node;
    FBCon con;
}FbconDeviceTreeNode;

void fbcon_putchar(CharDev* dev, U8 c);
void fbcon_init(void);
void fbcon_set_color(U32 color);
void fbcon_add_to_device_tree(void);
void fbcon_flush(CharDev* dev);
#endif
