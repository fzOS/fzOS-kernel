#ifndef WINDOW
#define WINDOW
#include <types.h>
#include <drivers/console.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/threading/thread.h>
#include <common/linkedlist.h>
typedef enum {
    WINDOW_MODE_NORMAL      = 0x00000000,
    WINDOW_MODE_NO_MAXIMIZE = 0x00000001,
    WINDOW_MODE_NO_MINIMIZE = 0x00000002,
    WINDOW_MODE_NO_CLOSE    = 0x00000004,
    WINDOW_MODE_BORDERLESS  = 0x00000008,
} WindowMode;
typedef enum {
    WINDOW_STATUS_NORMAL    = 0x00000000,
    WINDOW_STATUS_HIDDEN    = 0x00000001,
    WINDOW_STATUS_INACTIVE  = 0x00000002,
    WINDOW_STATUS_MAXIMIZED = 0x00000004,
} WindowStatus;
#define DEFAULT_BACKGROUND_COLOR      0x00b0beda
#define WINDOW_CAPTION_COLOR_ACTIVE   0x00417b98
#define WINDOW_CAPTION_COLOR_INACTIVE 0x00585858
#define WINDOW_CAPTION_HEIGHT         32
#define WINDOW_TITLE_GLYPH_HEIGHT     24
#define WINDOW_TITLE_GLYPH_WIDTH      12
typedef enum {
    NO_BUTTON       = 0x00,
    CLOSE_BUTTON    = 0x01,
    MAXIMIZE_BUTTON = 0x02,
    MINIMIZE_BUTTON = 0x04,
} CaptionButton;
typedef struct {
    U64 window_id;
    I64 x;
    I64 y;
    U64 width;
    U64 height;
    U64 orig_width;
    U64 orig_height;
    U64 orig_x;
    U64 orig_y;
    char*          caption;
    CaptionButton  button[3];
    WindowStatus   status;
    WindowMode     mode;
    //Coldpoint compat.
    thread*        ui_thread;
    object*        event_receiver;
    CodeAttribute* code_on_resize;
    CodeAttribute* code_on_click;
    CodeAttribute* code_on_move;
    CodeAttribute* code_on_minimize;
    CodeAttribute* code_on_close;
    CodeAttribute* code_on_activate;
    CodeAttribute* code_on_inactivate;
    Array buffer;
} Window;
typedef struct {
    InlineLinkedListNode node;
    Window w;
} WindowInlineLinkedListNode;

#endif
