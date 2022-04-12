#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <drivers/chardev.h>

#define KEY_RELEASE (0x80)
#define KEY_LSHIFT (0x2a)
#define KEY_RSHIFT (0x36)
#define KEY_CAPS (0x3A)
void init_keyboard(void);
void keyboard_getkey(int);
U8 keyboard_getchar(struct CharDev*);
#endif
