#ifndef CHARDEV
#define CHARDEV
#include <types.h>
//字符型设备的定义。
typedef struct {
    U8*(*getchar)(void);
    void(*putchar)(U8 data);
} char_dev;
//就先这两个吧。
#endif