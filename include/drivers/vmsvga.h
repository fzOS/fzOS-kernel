#ifndef VMSVGA_H
#define VMSVGA_H
#include <drivers/pci.h>
#define SVGA_FB_MAX_TRACEABLE_SIZE      0x1000000
#define SVGA_MAX_PSEUDOCOLOR_DEPTH      8
#define SVGA_MAX_PSEUDOCOLORS           (1 << SVGA_MAX_PSEUDOCOLOR_DEPTH)
#define SVGA_NUM_PALETTE_REGS           (3 * SVGA_MAX_PSEUDOCOLORS)
#define SVGA_BACK_BUFFER_ID             0
typedef struct {
    I32 left;
    I32 top;
    I32 right;
    I32 bottom;
} __attribute__((packed)) SVGASignedRect;
typedef union {
      struct {
         U32 bitsPerPixel : 8;
         U32 colorDepth   : 8;
         U32 reserved     : 16;  /* Must be zero */
      } split;
      U32 value;
}  __attribute__((packed)) SVGAGMRImageFormat;
typedef struct SVGAGuestMemDescriptor {
    U32 ppn;
    U32 numPages;
} __attribute__((packed)) SVGAGuestMemDescriptor;

typedef struct {
    I32 x;
    I32 y;
} __attribute__((packed)) SVGASignedPoint;
typedef struct SVGAGuestPtr {
    U32 gmrId;
    U32 offset;
} __attribute__((packed)) SVGAGuestPtr;
typedef enum {
    SVGA_REG_ID = 0,
    SVGA_REG_ENABLE = 1,
    SVGA_REG_WIDTH = 2,
    SVGA_REG_HEIGHT = 3,
    SVGA_REG_MAX_WIDTH = 4,
    SVGA_REG_MAX_HEIGHT = 5,
    SVGA_REG_DEPTH = 6,
    SVGA_REG_BITS_PER_PIXEL = 7,       /* Current bpp in the guest */
    SVGA_REG_PSEUDOCOLOR = 8,
    SVGA_REG_RED_MASK = 9,
    SVGA_REG_GREEN_MASK = 10,
    SVGA_REG_BLUE_MASK = 11,
    SVGA_REG_BYTES_PER_LINE = 12,
    SVGA_REG_FB_START = 13,            /* (Deprecated) */
    SVGA_REG_FB_OFFSET = 14,
    SVGA_REG_VRAM_SIZE = 15,
    SVGA_REG_FB_SIZE = 16,
    SVGA_REG_CAPABILITIES = 17,
    SVGA_REG_MEM_START = 18,           /* (Deprecated) */
    SVGA_REG_MEM_SIZE = 19,
    SVGA_REG_CONFIG_DONE = 20,         /* Set when memory area configured */
    SVGA_REG_SYNC = 21,                /* See "FIFO Synchronization Registers" */
    SVGA_REG_BUSY = 22,                /* See "FIFO Synchronization Registers" */
    SVGA_REG_GUEST_ID = 23,            /* Set guest OS identifier */
    SVGA_REG_CURSOR_ID = 24,           /* (Deprecated) */
    SVGA_REG_CURSOR_X = 25,            /* (Deprecated) */
    SVGA_REG_CURSOR_Y = 26,            /* (Deprecated) */
    SVGA_REG_CURSOR_ON = 27,           /* (Deprecated) */
    SVGA_REG_HOST_BITS_PER_PIXEL = 28, /* (Deprecated) */
    SVGA_REG_SCRATCH_SIZE = 29,        /* Number of scratch registers */
    SVGA_REG_MEM_REGS = 30,            /* Number of FIFO registers */
    SVGA_REG_NUM_DISPLAYS = 31,        /* (Deprecated) */
    SVGA_REG_PITCHLOCK = 32,           /* Fixed pitch for all modes */
    SVGA_REG_IRQMASK = 33,             /* Interrupt mask */
    SVGA_REG_NUM_GUEST_DISPLAYS = 34,/* Number of guest displays in X/Y direction */
    SVGA_REG_DISPLAY_ID = 35,        /* Display ID for the following display attributes */
    SVGA_REG_DISPLAY_IS_PRIMARY = 36,/* Whether this is a primary display */
    SVGA_REG_DISPLAY_POSITION_X = 37,/* The display position x */
    SVGA_REG_DISPLAY_POSITION_Y = 38,/* The display position y */
    SVGA_REG_DISPLAY_WIDTH = 39,     /* The display's width */
    SVGA_REG_DISPLAY_HEIGHT = 40,    /* The display's height */
    SVGA_REG_GMR_ID = 41,
    SVGA_REG_GMR_DESCRIPTOR = 42,
    SVGA_REG_GMR_MAX_IDS = 43,
    SVGA_REG_GMR_MAX_DESCRIPTOR_LENGTH = 44,
    SVGA_REG_TRACES = 45,            /* Enable trace-based updates even when FIFO is on */
    SVGA_REG_TOP = 46,               /* Must be 1 more than the last register */

    SVGA_PALETTE_BASE = 1024,        /* Base of SVGA color map */
    SVGA_SCRATCH_BASE = SVGA_PALETTE_BASE + SVGA_NUM_PALETTE_REGS
                                        /* Base of scratch registers */
    /* Next reg[SVGA_REG_SCRATCH_SIZE] registers exist for scratch usage:
        First 4 are reserved for VESA BIOS Extension; any remaining are for
        the use of the current SVGA driver. */
} VMSVGAIORegisters;
typedef enum {
    SVGA_FIFO_MIN = 0,
    SVGA_FIFO_MAX,       /* The distance from MIN to MAX must be at least 10K */
    SVGA_FIFO_NEXT_CMD,
    SVGA_FIFO_STOP,
    SVGA_FIFO_CAPABILITIES = 4,
    SVGA_FIFO_FLAGS,
    SVGA_FIFO_FENCE,
    SVGA_FIFO_3D_HWVERSION,
    SVGA_FIFO_PITCHLOCK,
    SVGA_FIFO_CURSOR_ON,          /* Cursor bypass 3 show/hide register */
    SVGA_FIFO_CURSOR_X,           /* Cursor bypass 3 x register */
    SVGA_FIFO_CURSOR_Y,           /* Cursor bypass 3 y register */
    SVGA_FIFO_CURSOR_COUNT,       /* Incremented when any of the other 3 change */
    SVGA_FIFO_CURSOR_LAST_UPDATED,/* Last time the host updated the cursor */
    SVGA_FIFO_RESERVED,           /* Bytes past NEXT_CMD with real contents */
    SVGA_FIFO_CURSOR_SCREEN_ID,
    SVGA_FIFO_3D_CAPS      = 32,
    SVGA_FIFO_3D_CAPS_LAST = 32 + 255,
    SVGA_FIFO_GUEST_3D_HWVERSION, /* Guest driver's 3D version */
    SVGA_FIFO_FENCE_GOAL,         /* Matching target for SVGA_IRQFLAG_FENCE_GOAL */
    SVGA_FIFO_BUSY,               /* See "FIFO Synchronization Registers" */
    SVGA_FIFO_NUM_REGS
} VMSVGAFIFORegisters;
typedef enum {
    SVGA_CAP_NONE             = 0x00000000,
    SVGA_CAP_RECT_COPY        = 0x00000002,
    SVGA_CAP_CURSOR           = 0x00000020,
    SVGA_CAP_CURSOR_BYPASS    = 0x00000040,   // Legacy (Use Cursor Bypass 3 instead)
    SVGA_CAP_CURSOR_BYPASS_2  = 0x00000080,   // Legacy (Use Cursor Bypass 3 instead)
    SVGA_CAP_8BIT_EMULATION   = 0x00000100,
    SVGA_CAP_ALPHA_CURSOR     = 0x00000200,
    SVGA_CAP_3D               = 0x00004000,
    SVGA_CAP_EXTENDED_FIFO    = 0x00008000,
    SVGA_CAP_MULTIMON         = 0x00010000,   // Legacy multi-monitor support
    SVGA_CAP_PITCHLOCK        = 0x00020000,
    SVGA_CAP_IRQMASK          = 0x00040000,
    SVGA_CAP_DISPLAY_TOPOLOGY = 0x00080000,   // Legacy multi-monitor support
    SVGA_CAP_GMR              = 0x00100000,
    SVGA_CAP_TRACES           = 0x00200000
} VMSVGACaps;
typedef enum {
    SVGA_FIFO_CAP_NONE            =     0 ,
    SVGA_FIFO_CAP_FENCE           = (1<<0),
    SVGA_FIFO_CAP_ACCELFRONT      = (1<<1),
    SVGA_FIFO_CAP_PITCHLOCK       = (1<<2),
    SVGA_FIFO_CAP_VIDEO           = (1<<3),
    SVGA_FIFO_CAP_CURSOR_BYPASS_3 = (1<<4),
    SVGA_FIFO_CAP_ESCAPE          = (1<<5),
    SVGA_FIFO_CAP_RESERVE         = (1<<6),
    SVGA_FIFO_CAP_SCREEN_OBJECT   = (1<<7),
    SVGA_FIFO_CAP_GMR2            = (1<<8),
    SVGA_FIFO_CAP_SCREEN_OBJECT_2 = (1<<9)
} VMSVGAFIFOCaps;
typedef enum {
    SVGA_CMD_INVALID_CMD           = 0,
    SVGA_CMD_UPDATE                = 1,
    SVGA_CMD_RECT_COPY             = 3,
    SVGA_CMD_DEFINE_CURSOR         = 19,
    SVGA_CMD_DEFINE_ALPHA_CURSOR   = 22,
    SVGA_CMD_UPDATE_VERBOSE        = 25,
    SVGA_CMD_FRONT_ROP_FILL        = 29,
    SVGA_CMD_FENCE                 = 30,
    SVGA_CMD_ESCAPE                = 33,
    SVGA_CMD_DEFINE_SCREEN         = 34,
    SVGA_CMD_DESTROY_SCREEN        = 35,
    SVGA_CMD_DEFINE_GMRFB          = 36,
    SVGA_CMD_BLIT_GMRFB_TO_SCREEN  = 37,
    SVGA_CMD_BLIT_SCREEN_TO_GMRFB  = 38,
    SVGA_CMD_ANNOTATION_FILL       = 39,
    SVGA_CMD_ANNOTATION_COPY       = 40,
    SVGA_CMD_MAX
} SVGAFifoCmdId;
typedef struct {
    U32 command;
    U32 x;
    U32 y;
    U32 width;
    U32 height;
} __attribute__((packed)) SVGAFifoCmdUpdate;
typedef struct {
    U32 command;
    U32 srcX;
    U32 srcY;
    U32 destX;
    U32 destY;
    U32 width;
    U32 height;
} __attribute__((packed)) SVGAFifoCmdRectCopy;
typedef struct {
    U32 command;
    U32 id;             // Reserved, must be zero.
    U32 hotspotX;
    U32 hotspotY;
    U32 width;
    U32 height;
} __attribute__((packed)) SVGAFifoCmdDefineAlphaCursor;
typedef struct {
    U32                 command;
    SVGAGuestPtr        ptr;
    U32                 bytesPerLine;
    SVGAGMRImageFormat  format;
} __attribute__((packed)) SVGAFifoCmdDefineGMRFB;
typedef struct {
    U32              command;
    SVGASignedPoint  srcOrigin;
    SVGASignedRect   destRect;
    U32              destScreenId;
} __attribute__((packed)) SVGAFifoCmdBlitGMRFBToScreen;

#define SVGA_INDEX_PORT         0x0
#define SVGA_VALUE_PORT         0x1
void vmsvga_register(U8 bus,U8 slot,U8 func);
void vmsvga_create_cursor(void);
void vmsvga_set_cursor_pos(U32 x,U32 y);
void vmsvga_console_move_up(int delta);
void vmsvga_update_screen(U32 x,U32 y,U32 width,U32 height);
void vmsvga_refresh_whole_screen(void);
extern volatile int g_screen_dirty;
#endif
