#ifndef CHARDEV
#define CHARDEV
#include <types.h>
//字符型设备的定义。
typedef struct CharDev{
    U8(*getchar)(struct CharDev*);
    void(*putchar)(struct CharDev*,U8 data);
    void(*flush)(struct CharDev*);
} CharDev;
//就先这两个吧。
#endif
