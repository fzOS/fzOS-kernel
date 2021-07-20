#ifndef HALT
#define HALT
#include <interrupt/interrupt.h>
void halt(void);
void die(char* reason,interrupt_frame* frame);
#endif
