#include <drivers/vmsvga.h>
#include <common/io.h>
#include <drivers/graphics.h>
#include <zcrystal/cursor.h>
#include <memory/memory.h>
static U64 g_io_port_base;
static U32* g_fifo_address;
static U64 g_fifo_length;
static const U32 g_fifo_begin_offset = SVGA_FIFO_NUM_REGS*sizeof(U32);
volatile int g_screen_dirty;
void vmsvga_write_register(U32 index,U32 value)
{
   outl(g_io_port_base + SVGA_INDEX_PORT, index);
   outl(g_io_port_base + SVGA_VALUE_PORT, value);
}
U32 vmsvga_read_register(U32 index)
{
   outl(g_io_port_base + SVGA_INDEX_PORT, index);
   return inl(g_io_port_base + SVGA_VALUE_PORT);
}
U32 vmsvga_inqueue(const U32* data,U32 length)
{
    U64 reserved = g_fifo_address[SVGA_FIFO_RESERVED];
    g_fifo_address[SVGA_FIFO_RESERVED] += length;
    U64 orig_pos = g_fifo_address[SVGA_FIFO_STOP]+reserved;
    U64 new_pos = (orig_pos+length-g_fifo_begin_offset)%g_fifo_length+g_fifo_begin_offset;
    I64 exceed_size = (orig_pos+length-g_fifo_begin_offset)-g_fifo_length;
    if(exceed_size<0) {
        exceed_size = 0;
    }
    memcpy((void*)((void*)g_fifo_address+orig_pos),data,(length-exceed_size));
    memcpy((void*)((void*)g_fifo_address+g_fifo_begin_offset),data+(length-exceed_size),exceed_size);
    g_fifo_address[SVGA_FIFO_NEXT_CMD] = new_pos;
    g_fifo_address[SVGA_FIFO_RESERVED] = reserved;
    return new_pos;
}
void vmsvga_register(U8 bus,U8 slot,U8 func)
{
    U16 vendor = pci_get_vendor(bus,slot,func);
    U16 device = pci_get_device(bus,slot,func);
    if(vendor!=0x15AD||device!=0x0405) {
        return;
    }
    printk(" VMSVGA:Got VMSVGA II at PCI bus %d,slot %d,func %d.\n",bus,slot,func);
    g_io_port_base = (U64)pci_get_bar_address(bus,slot,func,0)&0xFFFFFFF0;
    g_fifo_address = (U32*)((U64)(pci_get_bar_address(bus,slot,func,2)&0xFFFFFFF0)|KERNEL_ADDR_OFFSET);
    U32 fifo_size = vmsvga_read_register(SVGA_REG_MEM_SIZE);
    vmsvga_write_register(SVGA_REG_ENABLE, TRUE);
    vmsvga_write_register(SVGA_REG_BITS_PER_PIXEL,32);
    vmsvga_write_register(SVGA_REG_DISPLAY_WIDTH,g_graphics_data.pixels_per_line);
    vmsvga_write_register(SVGA_REG_DISPLAY_HEIGHT,g_graphics_data.pixels_vertical);
    vmsvga_write_register(SVGA_REG_DISPLAY_POSITION_X,0);
    vmsvga_write_register(SVGA_REG_DISPLAY_POSITION_Y,0);
    g_fifo_address[SVGA_FIFO_MIN] = g_fifo_begin_offset;
    g_fifo_address[SVGA_FIFO_MAX] = fifo_size;
    g_fifo_length = fifo_size-g_fifo_begin_offset;
    g_fifo_address[SVGA_FIFO_NEXT_CMD] = g_fifo_address[SVGA_FIFO_MIN];
    g_fifo_address[SVGA_FIFO_STOP] = g_fifo_address[SVGA_FIFO_MIN];
    g_fifo_address[SVGA_FIFO_RESERVED] = 0;
    U32 fifo_cap = g_fifo_address[SVGA_FIFO_CAPABILITIES];
    U32 reg_cap  = vmsvga_read_register(SVGA_REG_CAPABILITIES);
    if(fifo_cap & SVGA_FIFO_CAP_CURSOR_BYPASS_3) {
        printk(" VMSVGA:Hardware cursor is supported.\n");
        g_cursor_accelerated = TRUE;
    }
    if(reg_cap & SVGA_CAP_RECT_COPY) {
        printk(" VMSVGA:Hardware rect copy is supported.\n");
        g_move_up_accelerated = TRUE;
    }
    vmsvga_write_register(SVGA_REG_CONFIG_DONE, TRUE);
    vmsvga_create_cursor();
    return;
}
void vmsvga_create_cursor(void)
{
    vmsvga_inqueue((const U32*)&g_cursor_image,sizeof(g_cursor_image));
}
void vmsvga_set_cursor_pos(U32 x,U32 y)
{
    g_fifo_address[SVGA_FIFO_CURSOR_ON] = TRUE;
    g_fifo_address[SVGA_FIFO_CURSOR_X]  = x;
    g_fifo_address[SVGA_FIFO_CURSOR_Y]  = y;
    g_fifo_address[SVGA_FIFO_CURSOR_COUNT]++;
    g_screen_dirty = 1;
}
void vmsvga_refresh_whole_screen(void)
{
    if(g_screen_dirty) {
        vmsvga_update_screen(0,0,g_graphics_data.pixels_per_line,g_graphics_data.pixels_vertical);
        g_screen_dirty = 0;
    }
}
void vmsvga_update_screen(U32 x,U32 y,U32 width,U32 height)
{
    SVGAFifoCmdUpdate update_command = {
        .command = SVGA_CMD_UPDATE,
        .x       = x,
        .y       = y,
        .width   = width,
        .height  = height
    };
    vmsvga_inqueue((const U32*)&update_command,sizeof(update_command));
}
void vmsvga_console_move_up(int delta)
{
    SVGAFifoCmdRectCopy copy_command = {
        .command = SVGA_CMD_RECT_COPY,
        .destX = 0,
        .destY = 0,
        .srcX = 0,
        .srcY = delta,
        .width = g_graphics_data.pixels_per_line,
        .height = g_graphics_data.pixels_vertical-delta
    };
    vmsvga_inqueue((const U32*)&copy_command,sizeof(copy_command));
}
