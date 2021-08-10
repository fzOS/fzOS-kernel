#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <drivers/chardev.h>

#define KEY_RELEASE (0x80)
void init_keyboard(void);
void keyboard_getkey(int);

#endif
