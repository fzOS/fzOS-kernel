#ifndef HALT
#define HALT
#include <interrupt.h>
void halt(void);
void die(char* reason,interrupt_frame* frame);
#endif