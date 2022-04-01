#ifndef WINDOW
#define WINDOW
#include <types.h>
#include <drivers/console.h>
#include <coldpoint/heap/heap.h>
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
} WindowStatus;
#define DEFAULT_BACKGROUND_COLOR      0x00b0beda
#define WINDOW_CAPTION_COLOR_ACTIVE   0x00417b98
#define WINDOW_CAPTION_COLOR_INACTIVE 0x00585858
#define WINDOW_CAPTION_HEIGHT         32
typedef struct {
    U64 window_id;
    U64 x;
    U64 y;
    U64 width;
    U64 height;
    char* caption;
    WindowStatus status;
    WindowMode   mode;
    //Coldpoint compat.
    object* event_receiver;
    Array buffer;
} Window;
typedef struct {
    InlineLinkedListNode node;
    Window w;
} WindowInlineLinkedListNode;

#endif
