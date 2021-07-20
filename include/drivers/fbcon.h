#ifndef FBCON
#define FBCON
#include <drivers/console.h>
#include <drivers/graphics.h>
#include <drivers/asciifont.h>
typedef console fbcon;
void fbcon_init(fbcon* buff);
#endif
