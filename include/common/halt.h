#ifndef HALT
#define HALT
#include <interrupt/interrupt.h>
void halt(void);
void die(char* reason,InterruptFrame* frame);
#endif
