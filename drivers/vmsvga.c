#include <drivers/vmsvga.h>
#include <common/io.h>
#include <drivers/graphics.h>
#include <zcrystal/cursor.h>
#include <zcrystal/window_manager.h>
#include <memory/memory.h>
static U64 g_io_port_base;
static U32* g_fifo_address;
static U64 g_fifo_length;
static const U32 g_fifo_begin_offset = SVGA_FIFO_NUM_REGS*sizeof(U32);
volatile int g_screen_dirty;
int g_dual_buffer_enabled;
void vmsvga_define_back_buffer(void* addr,U64 pages);
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
    vmsvga_create_cursor();
    if(reg_cap & SVGA_CAP_RECT_COPY) {
        printk(" VMSVGA:Hardware rect copy is supported.\n");
        g_move_up_accelerated = TRUE;
    }
#pragma message "Why can't double buffer work?????"
#if 0
    if(fifo_cap & SVGA_FIFO_CAP_SCREEN_OBJECT_2) {
        printk(" VMSVGA:Double framebuffer is supported.\n");
        //Create a 2nd display area to prevent flicker.
        U64 page_count = (g_graphics_data.pixels_per_line*g_graphics_data.pixels_vertical*sizeof(U32))/PAGE_SIZE+1;
        void* alternative_memory_region = allocate_page(page_count);
        vmsvga_define_back_buffer(alternative_memory_region,page_count);
        g_graphics_data.frame_buffer_base = alternative_memory_region;
        g_dual_buffer_enabled = 1;
    }
#endif
    vmsvga_write_register(SVGA_REG_CONFIG_DONE, TRUE);
    return;
}
U32 GMR_AllocDescriptor(SVGAGuestMemDescriptor *descArray,U32 numDescriptors)
{
   const U32 descPerPage = PAGE_SIZE / sizeof(SVGAGuestMemDescriptor) - 1;
   SVGAGuestMemDescriptor *desc = NULL;
   U32 firstPage = 0;
   U32 page = 0;
   int i = 0;

   while (numDescriptors) {
      if (!firstPage) {
         firstPage = page = ((U64)allocate_page(1)&(~KERNEL_ADDR_OFFSET))/PAGE_SIZE;
      }

      desc = (void*)((page*PAGE_SIZE)|KERNEL_ADDR_OFFSET);

      if (i == descPerPage) {
         /*
          * Terminate this page with a pointer to the next one.
          */
         page = ((U64)allocate_page(1)&(~KERNEL_ADDR_OFFSET))/PAGE_SIZE;
         desc[i].ppn = page;
         desc[i].numPages = 0;
         i = 0;
         continue;
      }

      desc[i] = *descArray;
      i++;
      descArray++;
      numDescriptors--;
   }

   if (desc) {
      /* Terminate the end of the descriptor list. */
      desc[i].ppn = 0;
      desc[i].numPages = 0;
   }

   return firstPage;
}
void GMR_Define(U32 gmrId,
           SVGAGuestMemDescriptor *descArray,
           U32 numDescriptors)
{
   U32 desc = GMR_AllocDescriptor(descArray, numDescriptors);
   vmsvga_write_register(SVGA_REG_GMR_ID, gmrId);
   vmsvga_write_register(SVGA_REG_GMR_DESCRIPTOR, desc);

   if (desc) {
      /*
       * Clobber the first page, to verify that the device reads our
       * descriptors synchronously when we write the GMR registers.
       */
//       free_page((void*)((desc*PAGE_SIZE)|KERNEL_ADDR_OFFSET),1);
   }
}
U32 GMR_DefineContiguous(U32 gmrId,void* data, U32 numPages)
{
   SVGAGuestMemDescriptor desc = {
      .ppn = ((U64)data&(~KERNEL_ADDR_OFFSET))/PAGE_SIZE,
      .numPages = numPages,
   };
   GMR_Define(gmrId, &desc, 1);
   return desc.ppn;
}
void vmsvga_define_back_buffer(void* addr,U64 pages)
{
    GMR_DefineContiguous(SVGA_BACK_BUFFER_ID,addr,pages);
    SVGAFifoCmdDefineGMRFB cmd = {
        .command = SVGA_CMD_DEFINE_GMRFB,
        .format = {
            .split.bitsPerPixel = 32,
            .split.colorDepth   = 24,
            .split.reserved     = 0
        },
        .bytesPerLine = g_graphics_data.pixels_per_line*sizeof(U32),
        .ptr = {
            .gmrId = SVGA_BACK_BUFFER_ID,
            .offset = 0
        }
    };
    vmsvga_inqueue((const U32*)&cmd,sizeof(cmd));
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
void vmsvga_transport_back_buffer(void)
{
    SVGAFifoCmdBlitGMRFBToScreen command = {
        .command = SVGA_CMD_BLIT_GMRFB_TO_SCREEN,
        .destRect= {
            .top  = 0,
            .left = 0,
            .bottom = g_graphics_data.pixels_vertical,
            .right  = g_graphics_data.pixels_per_line
        },
        .srcOrigin = {
            .x = 0,
            .y = 0
        },
        .destScreenId = 0
    };
    vmsvga_inqueue((const U32*)&command,sizeof(command));
}
void vmsvga_refresh_whole_screen(void)
{
    if(g_screen_dirty && !g_screen_lock) {
#if 0
        if(g_dual_buffer_enabled) {
            vmsvga_transport_back_buffer();
        }
#endif
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
