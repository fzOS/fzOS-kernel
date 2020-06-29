#ifndef FBCON
#define FBCON
#include <console.h>
#include <graphics.h>
#include <asciifont.h>
typedef console fbcon;
void fbcon_init(fbcon* buff);
#endif