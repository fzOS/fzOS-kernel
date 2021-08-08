#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <drivers/chardev.h>


void init_keyboard(void);
void keyboard_getkey(int);

#endif
