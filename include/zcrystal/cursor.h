#ifndef CURSOR_H
#define CURSOR_H
#include <types.h>
#include <drivers/vmsvga.h>
typedef struct {
    SVGAFifoCmdDefineAlphaCursor header;
    U32 data[17*24];
} __attribute__((packed)) CursorImage;
extern const CursorImage g_cursor_image;
extern int g_cursor_accelerated;
void create_cursor(void);
void set_cursor_pos(U32 x,U32 y);
#endif
